#include "xml_reader.h"
#include "simple_filter.h"
#include <string>
#include <stdio.h>

using namespace gr::swifi;

#define PROGNAME  "framefilter"


bool produce_samples;
char *filter_string;
FILE *dump_out_f, *samples_in_f, *samples_out_f;
char *dump_in_filename;

void help() {
  printf("Usage: %s <filter_string> <dump_file> [samples_file]\n", PROGNAME);
}

bool checkfile(const char *filename, const char *mode = "r") {
  FILE *f = fopen(filename, mode);
  if (f) {
    fclose(f);
    return true;
  }
  return false;
}

void parse_args(int argc, char **argv) {
  if (argc < 3) {
    help();
    exit(-1);
  }

  // get command line arguments
  filter_string = argv[1];
  dump_in_filename = argv[2];
  char *samples_in_filename = NULL;
  if (argc >= 4)
    samples_in_filename = argv[3];

  // check dump input file
  if (!checkfile(dump_in_filename)) {
    fprintf(stderr, "Dump file %s not found\n", dump_in_filename);
    exit(-1);
  }

  // check dump output file
  std::string dump_out_filename = std::string(dump_in_filename).append(".filtered");
  if (!checkfile(dump_out_filename.c_str(), "w")) {
    fprintf(stderr, "Cannot write dump output to %s\n", dump_out_filename.c_str());
    exit(-1);
  }

  // open dump output files
  dump_out_f = fopen(dump_out_filename.c_str(), "w");


  // check samples input file
  produce_samples = false;
  if (!checkfile(samples_in_filename)) {
    printf("Warning: samples input file is not specified, no samples output will be produced\n");
  }
  else {
    std::string samples_out_filename = std::string(samples_in_filename).append(".filtered");
    if (!checkfile(samples_out_filename.c_str(), "w")) {
      printf("Warning: Cannot write filtered samples to %s\n", samples_out_filename.c_str());
    }
    else {
      // open samples input and output files
      samples_in_f = fopen(samples_in_filename, "r");
      samples_out_f = fopen(samples_out_filename.c_str(), "w");
      produce_samples = samples_in_f && samples_out_f;
    }
  }
}

void write_samples(const XMLNode &frame, int frame_index) {
  unsigned long start_sample = 0, end_sample = 0;
  std::string value;
  
  if (frame.retrieve_grandchild_or_attr(value, "phy.sample@payload_start")) {
    start_sample = atol(value.c_str());
  }

  if (frame.retrieve_grandchild_or_attr(value, "phy.sample@payload_end")) {
    end_sample = atol(value.c_str());
  }

  if (end_sample <= start_sample) {
    // something went wrong
    fprintf(stderr, "Stop writing samples. Values of start and end samples incorrect: start = %lu, end = %lu\n",
      start_sample, end_sample);
    produce_samples = false;
    return;
  }

  // locate frame in samples input file
  if (fseek(samples_in_f, start_sample, SEEK_SET) != 0) {
    fprintf(stderr, "Cannot read samples at position %lu\n", start_sample);
    produce_samples = false;
    return;
  }

  // read samples
  char *samples = new char[(end_sample - start_sample) * 8];
  size_t nread = fread(samples, 8, end_sample - start_sample, samples_in_f);
  if (nread != end_sample - start_sample) {
    fprintf(stderr, "Cannot read all samples for the frame %d starting at position %lu\n",
      frame_index, start_sample);
    produce_samples = false;
    delete[] samples;
    return;
  }

  // write samples
  size_t nwrite = fwrite(samples, 8, nread, samples_out_f);
  if (nwrite != nread) {
    fprintf(stderr, "Cannot write samples for the frame %d\n", frame_index);
    produce_samples = false;
    delete[] samples;
    return;
  }

  delete[] samples;

  printf("Frame %d, write %lu samples from %lu to %lu\n",
    frame_index, nwrite, start_sample, end_sample);
}

int main(int argc, char **argv) {
  parse_args(argc, argv);

  // read dump file
  XMLNode frames = XMLReader::read(dump_in_filename, "allframes");

  // create frame filter
  simple_filter frame_filter(filter_string);

  // filtering
  for (int i = 0; i < frames.num_children(); i++) {
    XMLNode &frame = frames.get_child(i);
    if (frame_filter.filter(frame)) {
      // we got one frame
      fprintf(dump_out_f, "%s", frame.str().c_str());
      if (produce_samples) {
        write_samples(frame, i);
      }
    }
  }
}
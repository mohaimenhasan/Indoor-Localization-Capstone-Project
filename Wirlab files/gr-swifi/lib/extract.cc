#include "xml_reader.h"
#include "simple_filter.h"
#include <string>
#include <vector>
#include <stdio.h>

using namespace gr::swifi;

#define PROGNAME  "extract"


char *filter_string;
char *filename;
std::vector<char*> fields;

void help() {
  printf("Extract fields from XML capture file.\n");
  printf("Usage: %s <filename> <filter_string> <field1> [field2 ...]\n", PROGNAME);
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
  if (argc < 4) {
    help();
    exit(-1);
  }

  // get command line arguments
  filename = argv[1];
  filter_string = argv[2];
  fields.resize(argc - 3);
  for (int i = 0; i < fields.size(); i++)
    fields[i] = argv[i + 3];

  // check input file
  if (!checkfile(filename)) {
    fprintf(stderr, "XML file %s not found\n", filename);
    exit(-1);
  }
}


int main(int argc, char **argv) {
  parse_args(argc, argv);

  // read dump file
  XMLNode frames = XMLReader::read(filename, "allframes");

  // create frame filter
  simple_filter frame_filter(filter_string);

  std::string value;

  // filtering
  for (int i = 0; i < frames.num_children(); i++) {
    XMLNode &frame = frames.get_child(i);
    if (frame_filter.filter(frame)) {
      // we got one frame
      for (int j = 0; j < fields.size(); j++) {
        if (j > 0) printf(",");
        if (frame.retrieve_grandchild_or_attr(value, fields[j]))
          printf("%s", value.c_str());
      }
      printf("\n");
    }
  }
}
const fs = require('fs');
const request = require('request');
let ReceiverVal = require('../models/receiver.model');
let CSIVal = require('../models/csi.model');

exports.home = function(req, res, next){
    res.send("Analysis Controller Home")
};

exports.parseByTime = async function(req, res, next) {
    await allData()
        .then(data => {
            // once you have the data you want to parse it here
            // time - // year, month, day, hour, min, second
            console.log(req.body.day+" "+req.body.month+" "+req.body.year + " " + req.body.timeFrom);
            console.log(req.body.day+" "+req.body.month+" "+req.body.year + " " + req.body.timeTo);
            let timeFrom = new Date(req.body.day+" "+req.body.month+" "+req.body.year + " " + req.body.timeFrom);
            let timeTo = new Date(req.body.day+" "+req.body.month+" "+req.body.year + " " + req.body.timeTo);
            let retVal = [];
            for (let i in data){
                console.log(data[i].timestamp)
                console.log(timeFrom);
                console.log(timeTo);
                if (new Date(data[i].timestamp) >= timeFrom && new Date(data[i].timestamp) <= timeTo){
                    retVal.push(data[i]);
                }
            }
            res.send(retVal);
        })
};

async function allData() {
    try{
        return await ReceiverVal.find()
            .then(data => {
                return data;
            })
    }catch(err){
        console.log("Nothing could be received");
        return null;
    }
}

exports.getAllData = async function(req, res, next){
    await allData()
        .then(data => {res.send(data);});
};
/*
function readFile(inputStream){
    try{
        let remaining = '';
        inputStream.on('data', function(data){
            let file = data.toString();
            console.log(file)
            remaining += data;
            let index = remaining.indexOf('\n');
            let last = 0;
            while (index > -1){
                let line = remaining.substring(last, index);
                last = index + 1;
                func(line);
                index = remaining.indexOf('\n');
            }
            remaining = remaining.substring(last);

        });
    }catch(err){
        console.log("File reading error");
        console.log(err);
    }
}
 */

async function saveCSIData(csi_frame, csi_grid, csi_data){
    try{
        let newVal = new CSIVal({
            frame: csi_frame,
            grid: csi_grid,
            data: csi_data
        });
        await newVal.save(function(err){
            if (err){
                throw(err);
            }
        });
        return newVal;
    }
    catch(err){
        console.log(err);
    }
}

exports.addCSIData = async function (req, res, next){
    try{
        // need the csi file
        await request(req.body.filePath).pipe(fs.createWriteStream(req.body.fileName));
        request.get(req.body.filePath, function(err, response, body){
            if(!err && response.statusCode === 200){
                let chunk = body.toString();
                chunk = chunk.split("scrambler zero items!!!!!!!!!")[0];
                chunk = chunk.split("TEMP_FILLED");
                csi = [];
                for (let pieces in chunk){
                    if (pieces === '0'){
                        continue;
                    }

                    let each_frame = chunk[pieces];
                    let begin = each_frame.indexOf("\n"+"[[");
                    if (begin === -1){
                        continue;
                    }
                    each_frame = each_frame.substr(begin);
                    let csi_data = each_frame.split("CSI FRAME")[0];
                    let end_frame = each_frame.split("CSI FRAME")[1].indexOf("]]");
                    end_frame = end_frame+2;
                    let csi_frame = each_frame.split("CSI FRAME")[1].substr(0, end_frame);
                    let csi_grid = each_frame.split("GRID")[1];
                    end_frame = csi_grid.indexOf("]");
                    csi_grid = csi_grid.substr(0, end_frame);
                    saveCSIData(csi_frame, csi_grid, csi_data);
                }
                res.send("Done");
            }else{
                res.send("Error");
            }
        });
    }catch(err){
        console.log("Error: ", err);
        res.send("Error");
    }
};
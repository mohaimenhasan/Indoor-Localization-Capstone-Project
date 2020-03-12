const fs = require('fs');
const request = require('request');
let ReceiverVal = require('../models/receiver.model');
let CSIVal = require('../models/csi.model');
let AnalysisVal = require('../models/analysis.model');

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
            for (let i=0; i < data.length; ++i){
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

async function saveAnalysisData(gridData, from, to){
    try{
        let analVal = new AnalysisVal({
            position: gridData,
            timefrom: from,
            timeto: to
        });
        return await analVal.save(function(err){
            if (err){
                throw err;
            }
            return analVal;
        });
    }catch(err){
        console.log("Failed to store data");
        return null;
    }
}

exports.sendAnalysisData = async function(req, res, next){
    try{
        let timeFrom = new Date(req.body.from);
        let timeTo = new Date(req.body.to);
        await getAnalysisData(timeFrom, timeTo)
            .then(data => {
                if (data === undefined){
                    res.send([]);
                }
                else{
                    res.send(data);
                }
            });
    }catch(err){
        console.log("BAD INPUT !!!!!");
        let badData = [];
        res.send(badData);
    }
};

async function getAnalysisData(from, to){
    try{
        // console.log(from);
        // console.log(to);
        return await AnalysisVal.find()
            .then(data=>{
                //console.log(data);
                let vals = [];
                for(let i=0; i < data.length; ++i){
                    // console.log("----------");
                    // console.log(data[i].timefrom);
                    // console.log(data[i].timeto);
                    if (data[i].timefrom >= from && data[i].timeto <= to){
                        vals.push(data[i]);
                    }
                }
                return vals;
            })
    }catch (err) {
        console.log("ERROR");
        return undefined;
    }
}

exports.storeAnalysisData = async function(req, res, next){
    // sanity check
    try{
        let val = req.body.data;
        val = JSON.parse(val);
        let gridData = new Array(val["position"]);
        let timeFrom = new Date(val["timefrom"]);
        let timeTo = new Date(val["timeto"]);
        await saveAnalysisData(gridData, timeFrom, timeTo)
            .then(response => {
                if (response === null){
                    res.send("Data could not be stored");
                }else{
                    res.send("Completed");
                }
            })
    }catch(err){
        console.log("Check Error: ", err);
        res.send("Bad Data");
    }
};


async function saveCSIData(csi_frame, csi_grid, csi_data){
    try{
        let newVal = new CSIVal({
            frame: csi_frame,
            grid: csi_grid,
            data: csi_data
        });
        return await newVal.save(function(err){
            if (err){
                throw(err);
            }
            return newVal;
        });
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
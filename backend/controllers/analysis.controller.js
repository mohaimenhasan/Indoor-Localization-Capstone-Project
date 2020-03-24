let spawn = require("child_process").spawn;
let ReceiverVal = require('../models/receiver.model');
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

async function saveAnalysisData(gridData, from, to, griddim, receiver1Pos, receiver1aoa, receiver1line, receiver2Pos,
                                receiver2aoa, receiver2line, receiver3Pos, receiver3aoa, receiver3line, receiver4Pos,
                                receiver4aoa, receiver4line){
    try{
        let analVal = new AnalysisVal({
            position: gridData,
            timefrom: from,
            timeto: to,
            receivers: {
                receiver1: {
                    position: receiver1Pos,
                    angle_of_arrival: receiver1aoa,
                    line: receiver1line
                },
                receiver2: {
                    position: receiver2Pos,
                    angle_of_arrival: receiver2aoa,
                    line: receiver2line
                },
                receiver3: {
                    position: receiver3Pos,
                    angle_of_arrival: receiver3aoa,
                    line: receiver3line
                },
                receiver4: {
                    position: receiver4Pos,
                    angle_of_arrival: receiver4aoa,
                    line: receiver4line
                }
            },
            griddim: griddim
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
        let allReceivers = val["receivers"];
        let griddim = new Array(val["gridDim"]);

        console.log(griddim);
        await saveAnalysisData(gridData, timeFrom, timeTo, griddim,
            allReceivers["receiver0"]["position"], allReceivers["receiver0"]["angle_of_arrival"], allReceivers["receiver0"]["line"],
            allReceivers["receiver1"]["position"], allReceivers["receiver1"]["angle_of_arrival"], allReceivers["receiver1"]["line"],
            allReceivers["receiver2"]["position"], allReceivers["receiver2"]["angle_of_arrival"], allReceivers["receiver2"]["line"],
            allReceivers["receiver3"]["position"], allReceivers["receiver3"]["angle_of_arrival"], allReceivers["receiver3"]["line"])
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

async function getLatestDates(){
    try{
        return await AnalysisVal.findOne().sort({ field: 'asc', _id: -1 }).limit(1)
            .then(data => {
                console.log(data);
                return data;
            })
    }catch (e) {

    }
}

exports.runPythonCur = async function(req, res, next){
    try{
        let currentPath = process.cwd();
        let pythonPath = currentPath+'/../Position Calculation/CalculatePosition.py';
        const pythonProcess = spawn('python',[pythonPath]);
        pythonProcess.stdout.on('data', async (data) => {
            await getLatestDates()
                .then(data=>{
                    res.send(data);
                });
        });
    }catch(err){

    }
};

exports.getAllDates = async function (req, res, next) {
    try{
        let allDates= [];
        await AnalysisVal.find()
            .then(data =>{
                for (i in data){
                    allDates.push(data[i]['timefrom']);
                    allDates.push(data[i]['timeto']);
                }
                res.send({
                    dates: allDates}
                    );
            })
    }catch(err){
        console.log("Error with fetching dates");
        res.send([]);
    }
};


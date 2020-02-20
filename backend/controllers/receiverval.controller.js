let ReceiverVal = require('../models/receiver.model');
async function retrieveValues(accessPt, timestamp, AoA){
    try{
        let newVal = new ReceiverVal({
            access_point: accessPt,
            timestamp: timestamp,
            angle_of_arrival: AoA
        });

        await newVal.save(function(err){
            if (err){
                throw(err);
            }
        });
        return newVal;
    }catch(e){
        console.log(e);
        console.log("ERROR: Could not retrieve values");
        return null;
    }
}
exports.postVal = async function(req, res, next){
    console.log("The request is: ", req.body);
    let accessPt = req.body.access_point;
    let timestamp = req.body.timestamp;
    let AoA = req.body.angle_of_arrival;
    let val = await retrieveValues(accessPt, timestamp, AoA);
    if (val === null){
        res.send("ERROR DID NOT RECEIVE SHIT");
    }else{
        res.send({
            code: 200,
            message: val
        })
    }
}
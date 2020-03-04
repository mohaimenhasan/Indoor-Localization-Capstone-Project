let ReceiverVal = require('../models/receiver.model');

exports.home = function(req, res, next){
    res.send("Analysis Controller Home")
}

async function allData(DateTime) {
    try{
        return await ReceiverVal.find()
            .then(data => {
                console.log("Returning this: ", data);
                return data;
            })
    }catch(err){
        console.log("Nothing could be received");
        return null;
    }
}

exports.getAllData = async function(req, res, next){
    await allData(null)
        .then(data => {res.send(data);});
};
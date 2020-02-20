const mongoose = require('mongoose');
const Schema = mongoose.Schema;

let receiveSchema = new Schema({
    access_point: { type: Number, default: -1},
    timestamp: { type: Date, default: Date.now },
    angle_of_arrival: { type: Number}
});

module.exports = mongoose.model('receiverData', receiveSchema);
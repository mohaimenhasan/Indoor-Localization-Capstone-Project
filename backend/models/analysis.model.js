const mongoose = require('mongoose');
const Schema = mongoose.Schema;

let analysisSchema = new Schema({
    position: {type: Array, default: []},
    timefrom: {type: Date, default: Date.now },
    timeto: {type: Date, default: Date.now }
});

module.exports = mongoose.model('analysisData', analysisSchema);
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

let csiSchema = new Schema({
    timestamp: { type: Date, default: Date.now },
    frame: { type: Array, default: []},
    grid: {type: Array, default: []},
    data: {type: Array, default: []}
});

module.exports = mongoose.model('csiData', csiSchema);
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

let analysisSchema = new Schema({
    position: {type: Array, default: []},
    receivers: {
        receiver1: {
            position: {type: Array, default: []},
            angle_of_arrival: {type: Number},
            line: {type: String, default: null}
        },
        receiver2: {
            position: {type: Array, default: []},
            angle_of_arrival: {type: Number},
            line: {type: String, default: null}
        },
        receiver3: {
            position: {type: Array, default: []},
            angle_of_arrival: {type: Number},
            line: {type: String, default: null}
        },
        receiver4: {
            position: {type: Array, default: []},
            angle_of_arrival: {type: Number},
            line: {type: String, default: null}
        }
    },
    timefrom: {type: Date, default: Date.now },
    timeto: {type: Date, default: Date.now },
    griddim: {type: Array, default: []}
});

module.exports = mongoose.model('analysisData', analysisSchema);
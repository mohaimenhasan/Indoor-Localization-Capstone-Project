var express = require('express');
var router = express.Router();

let analysisController = require('../controllers/analysis.controller');
/* GET home page. */
router.get('/', analysisController.home);
router.get('/allData', analysisController.getAllData);
module.exports = router;

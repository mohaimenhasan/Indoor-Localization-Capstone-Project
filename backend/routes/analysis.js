var express = require('express');
var router = express.Router();

let analysisController = require('../controllers/analysis.controller');
/* GET home page. */
router.get('/', analysisController.home);
router.get('/allData', analysisController.getAllData);
router.get('/byTime', analysisController.parseByTime);
router.post('/storeAnalysisData', analysisController.storeAnalysisData);
router.post('/sendAnalysisData', analysisController.sendAnalysisData);
router.get('/getAllDates', analysisController.getAllDates);

module.exports = router;

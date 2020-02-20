var express = require('express');
var router = express.Router();

let receiverController = require('../controllers/receiverval.controller');
/* GET home page. */
router.post('/addData', receiverController.postVal);

module.exports = router;


var express = require('express');
var router = express.Router();

let indexController = require('../controllers/index.controller');
/* GET home page. */
router.get('/', indexController.home);
router.get('/test', indexController.test);

module.exports = router;

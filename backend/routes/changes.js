var express = require('express');
var router = express.Router();

let changesController = require('../controllers/changes.controller');
/* GET home page. */
router.get('/', changesController.home);
module.exports = router;


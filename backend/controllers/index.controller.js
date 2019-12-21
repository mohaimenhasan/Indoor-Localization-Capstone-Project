exports.test = function(req, res, next){
    res.send("Hello darkness my old friend");
};

exports.home = function (req, res, next) {
    res.render('index', { title: 'Express' });
};
import React, {Component} from "react";
import mo from './mo.png';
import mo_cont from './mo_cont.png';
import gps from './gps.png';
import indoor from './indoor.png';
import work from './work.png';

class Motivation extends Component{
    constructor(props) {
        super(props);
    }

    render(){
        return(
            <div>
                <img src={mo} alt="mo" />
                <img src={mo_cont} alt="mo_cont" />
                <img src={gps} alt="gps" />
                <img src={indoor} alt="indoor" />               
                <img src={work} alt="work" />
            </div>
        );
    }
}

export default Motivation
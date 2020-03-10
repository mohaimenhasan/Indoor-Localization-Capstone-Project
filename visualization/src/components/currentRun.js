import React, {Component} from 'react';
import HeatMap from "react-heatmap-grid";
import PositionData from "../sampleOut";
import { makeStyles } from '@material-ui/core/styles';

class currentRun extends Component{
    constructor(props){
        super(props);
        this.state = {

        }
    }

    render() {
        let positionMatrix = PositionData["position"];
        positionMatrix = Array.from(positionMatrix);
        const xLabels = new Array(positionMatrix.length).fill(0).map((_,i) => `${i}`);

        // Display only even labels
        const xLabelsVisibility = new Array(24)
            .fill(0)
            .map((_, i) => (i %1 === 0));

        const yLabels = new Array(positionMatrix[0].length).fill(0).map((_,i) => `${i}`);
        const data = positionMatrix;
        return(
            <div style={{ fontSize: "16px" }}>
                <HeatMap
                    height={75}
                    xLabels={xLabels}
                    yLabels={yLabels}
                    xLabelsLocation={"bottom"}
                    xLabelsVisibility={xLabelsVisibility}
                    xLabelWidth={60}
                    data={data}
                    squares
                    onClick={(x, y) => alert(`Clicked ${x}, ${y}`)}
                    cellStyle={(background, value, min, max, data, x, y) => ({
                        background: `rgb(230, 81, 0, ${1 + 0.1 - (max - value) / (max - min)})`,
                        fontSize: "15px",
                        color: "#000"
                    })}
                    cellRender={value => value && `${value}%`}
                />
            </div>
        )
    }
}

export default currentRun;
import React, {Component} from 'react';
import Dashboard from "./components/Dashboard";
import Current from "./components/currentRun";

import './App.css';

class App extends Component{
    constructor(props){
        super(props);
        this.state = {
            currentScreen: []
        }
    }

    componentDidMount() {
        this.setState({
            currentScreen: <Dashboard/>
        })
    }

    render(){
        return (
            <div className="App">
                {this.state.currentScreen}
            </div>
        );
    }
}

export default App;
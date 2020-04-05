import React, {Component} from "react";
import Typography from "@material-ui/core/Typography";
import Link from "@material-ui/core/Link";
import {makeStyles} from "@material-ui/core/styles";
import CssBaseline from "@material-ui/core/CssBaseline";
import AppBar from "@material-ui/core/AppBar/AppBar";
import clsx from "clsx";
import Toolbar from "@material-ui/core/Toolbar";
import IconButton from "@material-ui/core/IconButton";
import MenuIcon from "@material-ui/icons/Menu";
import Drawer from "@material-ui/core/Drawer";
import ChevronLeftIcon from "@material-ui/icons/ChevronLeft";
import Divider from "@material-ui/core/Divider";
import List from "@material-ui/core/List";
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import DashboardIcon from "@material-ui/icons/Dashboard";
import ListItemText from "@material-ui/core/ListItemText";
import Dashboard from "./Dashboard";
import Current from "./HowItWorks";
import DataUsageIcon from "@material-ui/icons/DataUsage";
import Container from "@material-ui/core/Container";
import Box from "@material-ui/core/Box";
import Grid from '@material-ui/core/Grid';
import Paper from '@material-ui/core/Paper';
import TextField from "@material-ui/core/TextField";
import MenuItem from "@material-ui/core/MenuItem";
import fetch from 'node-fetch';
import Button from "@material-ui/core/Button";
import PlayCircleOutlineIcon from "@material-ui/icons/PlayCircleOutline";
import Plot from "react-plotly.js";
import RunSimulation from "./RunSimulation";


function Copyright() {
    return (
        <Typography variant="body2" color="textSecondary" align="center">
            {'Copyright © '}
            <Link color="inherit" href="https://github.com/mohaimenhasan/Indoor-Localization-Capstone-Project">
                Mohaimen and Friends
            </Link>{' '}
            {new Date().getFullYear()}
            {'.'}
        </Typography>
    );
}

const drawerWidth = 240;

const api_base_url= "http://localhost:8888/";

const useStyles = makeStyles(theme => ({
    root: {
        display: 'flex',
    },
    toolbar: {
        paddingRight: 24, // keep right padding when drawer closed
    },
    toolbarIcon: {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'flex-end',
        padding: '0 8px',
        ...theme.mixins.toolbar,
    },
    heatmapex: {
        marginBottom: "15vh",
        fontSize: "16px",
        height: "60vh",
        width: "80vw",
        margin: "4rem auto"
    },
    appBar: {
        zIndex: theme.zIndex.drawer + 1,
        transition: theme.transitions.create(['width', 'margin'], {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen,
        }),
    },
    appBarShift: {
        marginLeft: drawerWidth,
        width: `calc(100% - ${drawerWidth}px)`,
        transition: theme.transitions.create(['width', 'margin'], {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.enteringScreen,
        }),
    },
    submit:{
        width: 100,
        margin: theme.spacing(1.5)
    },
    menuButton: {
        marginRight: 36,
    },
    menuButtonHidden: {
        display: 'none',
    },
    title: {
        flexGrow: 1,
    },
    drawerPaper: {
        position: 'relative',
        whiteSpace: 'nowrap',
        width: drawerWidth,
        transition: theme.transitions.create('width', {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.enteringScreen,
        }),
    },
    drawerPaperClose: {
        overflowX: 'hidden',
        transition: theme.transitions.create('width', {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen,
        }),
        width: theme.spacing(7),
        [theme.breakpoints.up('sm')]: {
            width: theme.spacing(9),
        },
    },
    appBarSpacer: theme.mixins.toolbar,
    content: {
        flexGrow: 1,
        height: '100vh',
        overflow: 'auto',
    },
    container: {
        paddingTop: theme.spacing(4),
        paddingBottom: theme.spacing(4),
    },
    paper: {
        padding: theme.spacing(2),
        display: 'flex',
        overflow: 'auto',
        flexDirection: 'column',
    },
    fixedHeight: {
        height: 240,
    },
    receiver1: {
        marginRight: "38vw",
    },
    receiver2: {
        marginRight: "0vw"
    },
    receiver3: {
        marginRight: "38vw"
    },
    receiver4: {
        marginRight: "0vw"
    }
}));

function withMyHook(Component){
    return function WrappedComponent(props){
        const classes = useStyles();
        return <Component {...props} classes={classes}/>
    }
}

class AnalysisReport extends Component{
    constructor(props){
        super(props);
        this.state = {
            open: true,
            fromDate: "",
            toDate: "",
            allDates: [],
            plotHeatMap: []
        }
    }
    handleDrawerOpen = () => {
        this.setState({
            open: true
        });
    };

    handleDrawerClose = () => {
        this.setState({
            open: false
        })
    };

    changeDashboard(event){
        this.props.appContext.setState({
            currentScreen: <Dashboard appContext={this.props.appContext}/>
        })
    }

    changeToCurrent(event){
        console.log("Current Screen picked");
        this.props.appContext.setState({
            currentScreen: <Current appContext={this.props.appContext}/>
        });
    }

    changeToRun(event){
        this.props.appContext.setState({
            currentScreen: <RunSimulation appContext={this.props.appContext}/>
        });
    }

    handleDateFrom = (event) => {
        this.setState({
            fromDate: event.target.value
        });
    };

    handleSubmit = async (event) => {
        const classes = this.props.classes;
        await fetch(api_base_url+'analysis/sendAnalysisData',
            {
                method: 'POST',
                headers: {
                    "Content-Type": "application/json",
                    "Access-Control-Allow-Origin": '*'
                },
                body: JSON.stringify({
                        from: this.state.fromDate,
                        to: this.state.toDate,
                    }
                )
            }).then(res => res.json())
            .then(response => {
                //console.log(data);
                let heatMaps = [];
                for (let k in response){
                    let positionMatrix = response[k]["position"][0];
                    let xDim = response[k]["griddim"][0][0];
                    let yDim = response[k]["griddim"][0][1];
                    positionMatrix = Array.from(positionMatrix);

                    let xLabels = [];
                    let yLabels = [];

                    for (let i=0; i < xDim; ++i){
                        xLabels.push(i);
                    }

                    for (let i=0; i < yDim; ++i){
                        yLabels.push(i);
                    }
                    const data = positionMatrix;
                    let r1x = [];
                    let r1y = [];
                    let r2x = [];
                    let r2y = [];
                    let r3x = [];
                    let r3y = [];
                    let r4x = [];
                    let r4y = [];

                    for (let i=0; i < xDim; i=i+0.01){
                        let y_val = response[k]["receivers"]["receiver1"]["line"]["slope"]*i+response[k]["receivers"]["receiver1"]["line"]["intercept"]/100.0;
                        if (y_val >= 0 && y_val <= yDim){
                            r1x.push(i);
                            r1y.push(response[k]["receivers"]["receiver1"]["line"]["slope"]*i+response[k]["receivers"]["receiver1"]["line"]["intercept"]/100.0);
                        }
                    }
                    for (let i=0; i < xDim; i=i+0.01){
                        let y_val = response[k]["receivers"]["receiver2"]["line"]["slope"]*i+response[k]["receivers"]["receiver2"]["line"]["intercept"]/100.0;
                        if (y_val >= 0 && y_val <= yDim){
                            r2x.push(i);
                            r2y.push(response[k]["receivers"]["receiver2"]["line"]["slope"]*i+response[k]["receivers"]["receiver2"]["line"]["intercept"]/100.0);
                        }
                    }

                    for (let i=0; i < xDim; i=i+0.01){
                        let y_val = response[k]["receivers"]["receiver3"]["line"]["slope"]*i+response[k]["receivers"]["receiver3"]["line"]["intercept"]/100.0;
                        if (y_val >= 0 && y_val <= yDim){
                            r3x.push(i);
                            r3y.push(response[k]["receivers"]["receiver3"]["line"]["slope"]*i+response[k]["receivers"]["receiver3"]["line"]["intercept"]/100.0);
                        }
                    }

                    for (let i=0; i < xDim; i=i+0.01){
                        let y_val = response[k]["receivers"]["receiver4"]["line"]["slope"]*i+response[k]["receivers"]["receiver4"]["line"]["intercept"]/100.0;
                        if (y_val >= 0 && y_val <= yDim){
                            r4x.push(i);
                            r4y.push(response[k]["receivers"]["receiver4"]["line"]["slope"]*i+response[k]["receivers"]["receiver4"]["line"]["intercept"]/100.0);
                        }
                    }


                    let temp = [];
                    temp.push(
                        <div className={classes.heatmapex}>
                            <Plot
                                data={[{
                                        z: data,
                                        x: xLabels,
                                        y: yLabels,
                                        type: "heatmap",
                                        colorscale: "Portland",
                                        hovertemplate: "Position: %{x}m, %{y}m"
                                    },
                                    {
                                        x: [(response[k]["receivers"]["receiver1"]["position"][0])/100.0],
                                        y: [(response[k]["receivers"]["receiver1"]["position"][1])/100.0],
                                        type: 'scatter',
                                        mode: 'markers',
                                        marker: {
                                            color: 'rgb(17, 157, 255)',
                                            size: 30,
                                            line: {
                                                color: 'rgb(231, 99, 250)',
                                                width: 6
                                            },
                                            symbol: 'asterisk'
                                        },
                                        showlegend: false,
                                        name: 'Receiver 1',
                                        hovertemplate: "Position: %{x}m, %{y}m"
                                    },
                                    {
                                        x: [(response[k]["receivers"]["receiver2"]["position"][0])/100.0],
                                        y: [(response[k]["receivers"]["receiver2"]["position"][1])/100.0],
                                        type: 'scatter',
                                        mode: 'markers',
                                        marker: {
                                            color: 'rgb(17, 157, 255)',
                                            size: 30,
                                            line: {
                                                color: 'rgb(231, 99, 250)',
                                                width: 6
                                            },
                                            symbol: 'asterisk'
                                        },
                                        showlegend: false,
                                        name: 'Receiver 2',
                                        hovertemplate: "Position: %{x}m, %{y}m"
                                    },
                                    {
                                        x: [(response[k]["receivers"]["receiver3"]["position"][0])/100.0],
                                        y: [(response[k]["receivers"]["receiver3"]["position"][1])/100.0],
                                        type: 'scatter',
                                        mode: 'markers',
                                        marker: {
                                            color: 'rgb(17, 157, 255)',
                                            size: 30,
                                            line: {
                                                color: 'rgb(231, 99, 250)',
                                                width: 6
                                            },
                                            symbol: 'asterisk'
                                        },
                                        showlegend: false,
                                        name: 'Receiver 3',
                                        hovertemplate: "Position: %{x}m, %{y}m"
                                    },
                                    {
                                        x: [(response[k]["receivers"]["receiver4"]["position"][0])/100.0],
                                        y: [(response[k]["receivers"]["receiver4"]["position"][1])/100.0],
                                        type: 'scatter',
                                        mode: 'markers',
                                        marker: {
                                            color: 'rgb(17, 157, 255)',
                                            size: 30,
                                            line: {
                                                color: 'rgb(231, 99, 250)',
                                                width: 6
                                            },
                                            symbol: 'asterisk'
                                        },
                                        showlegend: false,
                                        name: 'Receiver 4',
                                        hovertemplate: "Position: %{x}m, %{y}m"
                                    },
                                    {
                                        x: r1x,
                                        y: r1y,
                                        marker: {
                                            color: 'rgb(255, 255, 255)',
                                        },
                                        mode: 'lines',
                                        showlegend: false
                                    },
                                    {
                                        x: r2x,
                                        y: r2y,
                                        marker: {
                                            color: 'rgb(255, 255, 255)',
                                        },
                                        mode: 'lines',
                                        showlegend: false
                                    },
                                    {
                                        x: r3x,
                                        y: r3y,
                                        marker: {
                                            color: 'rgb(255, 255, 255)',
                                        },
                                        mode: 'lines',
                                        showlegend: false
                                    },
                                    {
                                        x: r4x,
                                        y: r4y,
                                        marker: {
                                            color: 'rgb(255, 255, 255)',
                                        },
                                        mode: 'lines',
                                        showlegend: false
                                    }
                                ]}
                                layout={{
                                    width: "80vw",
                                    height: "60vh",
                                    title: "Timeline: <b>"+response[k]["timefrom"]+"</b> to <b>"+response[k]["timeto"]+"</b>",
                                    font: {
                                        size: "20px"
                                    }}}
                            />
                            <br/>
                        </div>
                    );
                    heatMaps.push(temp);
                }

                this.setState({
                    plotHeatMap: heatMaps
                });
            })
    };

    handleDateTo = (event) => {
        if (this.state.fromDate === ""){
            alert("Select a time from first");
            return;
        }
        if (event.target.value < this.state.fromDate){
            alert("Time to needs to be larger than time from");
            return;
        }
        this.setState({
            toDate: event.target.value
        })
    };

    componentDidMount = async function () {
        await fetch(api_base_url+'analysis/getAllDates',
            {
                method: 'GET',
                headers: {
                    "Content-Type": "application/json",
                    "Access-Control-Allow-Origin": '*'
                }
            })
            .then(response=>response.json())
            .then(data => {
                let vals = [];
                for (let i in data["dates"]){
                    vals.push({
                        value: data["dates"][i],
                        label: data["dates"][i]
                    })
                }
                this.setState({
                    allDates: vals
                })
        })
    };

    render() {
        const classes = this.props.classes;
        const mainListItems = (
            <div>
                <ListItem button onClick={(event) => this.changeDashboard(event)}>
                    <ListItemIcon>
                        <DashboardIcon />
                    </ListItemIcon>
                    <ListItemText primary="Dashboard" />
                </ListItem>
                <ListItem button onClick={(event) => this.changeToCurrent(event)}>
                    <ListItemIcon>
                        <DataUsageIcon />
                    </ListItemIcon>
                    <ListItemText primary="How it works" />
                </ListItem>
                <ListItem button onClick={(event) => this.changeToRun(event)}>
                    <ListItemIcon>
                        <PlayCircleOutlineIcon/>
                    </ListItemIcon>
                    <ListItemText primary="Run Simulation" />
                </ListItem>
            </div>
        );

        const fixedHeightPaper = clsx(classes.paper, classes.fixedHeight);


        return(
            <div className={classes.root}>
            <CssBaseline />
            <AppBar position="absolute" className={clsx(classes.appBar, this.state.open && classes.appBarShift)}>
                <Toolbar className={classes.toolbar}>
                    <IconButton
                        edge="start"
                        color="inherit"
                        aria-label="open drawer"
                        onClick={this.handleDrawerOpen}
                        className={clsx(classes.menuButton, this.state.open && classes.menuButtonHidden)}
                    >
                        <MenuIcon />
                    </IconButton>
                    <Typography component="h1" variant="h6" color="inherit" noWrap className={classes.title}>
                        Analysis Report
                    </Typography>
                </Toolbar>
            </AppBar>
            <Drawer
                variant="permanent"
                classes={{
                    paper: clsx(classes.drawerPaper, !this.state.open && classes.drawerPaperClose),
                }}
                open={this.state.open}
            >
                <div className={classes.toolbarIcon}>
                    <IconButton onClick={this.handleDrawerClose}>
                        <ChevronLeftIcon />
                    </IconButton>
                </div>
                <Divider />
                <List>{mainListItems}</List>
                <Divider />
            </Drawer>
            <main className={classes.content}>
                <div className={classes.appBarSpacer} />
                <Container maxWidth="lg" className={classes.container}>
                    <Grid container spacing={3}>
                        <Grid item xs={12} md={12} lg={12}>
                            <Paper className={fixedHeightPaper}>
                                <TextField
                                    id="standard-select-currency"
                                    select
                                    label="Time From"
                                    value={this.state.fromDate}
                                    onChange={this.handleDateFrom}
                                    helperText="Please select date from"
                                >
                                    {this.state.allDates.map(option => (
                                        <MenuItem key={option.value} value={option.value}>
                                            {option.label}
                                        </MenuItem>
                                    ))}
                                </TextField>
                                <TextField
                                    id="standard-select-currency"
                                    select
                                    label="Time To"
                                    value={this.state.toDate}
                                    onChange={this.handleDateTo}
                                    helperText="Please select date to"
                                >
                                    {this.state.allDates.map(option => (
                                        <MenuItem key={option.value} value={option.value}>
                                            {option.label}
                                        </MenuItem>
                                    ))}
                                </TextField>
                                <div style={{
                                    alignItems: "center"
                                }}>
                                    <Button variant="outlined" size="large" color="primary" className={classes.submit} onClick={this.handleSubmit}>
                                        Submit
                                    </Button>
                                </div>
                            </Paper>
                        </Grid>
                    </Grid>
                    {this.state.plotHeatMap}
                    <Box pt={4}>
                        <Copyright />
                    </Box>
                </Container>
            </main>
        </div>);
    }
}

AnalysisReport = withMyHook(AnalysisReport);
export default AnalysisReport;
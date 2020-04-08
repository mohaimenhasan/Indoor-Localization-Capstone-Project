import React, {Component} from 'react';
import CssBaseline from "@material-ui/core/CssBaseline";
import clsx from "clsx";
import Toolbar from "@material-ui/core/Toolbar";
import IconButton from "@material-ui/core/IconButton";
import Typography from "@material-ui/core/Typography";
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import ListItemText from "@material-ui/core/ListItemText";
import Divider from "@material-ui/core/Divider";
import List from "@material-ui/core/List";
import Drawer from "@material-ui/core/Drawer";
import Dashboard from "./Dashboard";
import Container from "@material-ui/core/Container";
import Link from "@material-ui/core/Link";
import Box from "@material-ui/core/Box";
import { makeStyles } from '@material-ui/core/styles';
import AppBar from "@material-ui/core/AppBar/AppBar";
import MenuIcon from '@material-ui/icons/Menu';
import DashboardIcon from '@material-ui/icons/Dashboard';
import BarChartIcon from '@material-ui/icons/BarChart';
import ChevronLeftIcon from "@material-ui/icons/ChevronLeft";
import AnalysisReport from "./AnalysisReport";
import PlayCircleOutlineIcon from "@material-ui/icons/PlayCircleOutline";
import Plot from 'react-plotly.js';
import Grid from "@material-ui/core/Grid";
import PlayArrowIcon from '@material-ui/icons/PlayArrow';
import Paper from "@material-ui/core/Paper";
import Button from "@material-ui/core/Button";
import fetch from "node-fetch";
import LinearProgress from "@material-ui/core/LinearProgress";
import {lighten} from "@material-ui/core";
import withStyles from "@material-ui/core/styles/withStyles";

let drawerWidth = 240;
let useStyles;

const api_base_url= "http://localhost:8888/";
const BorderLinearProgress = withStyles({
    root: {
        height: 10,
        backgroundColor: lighten('#ff6c5c', 0.5),
    },
    bar: {
        borderRadius: 20,
        backgroundColor: '#ff6c5c',
    },
})(LinearProgress);

useStyles = makeStyles(theme => ({
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
        height: 70,
    },
    heatmapex: {
        fontSize: "16px",
        height: "60vh",
        width: "80vw",
        margin: "4rem auto"
    },
    iconsTop: {

    },
    iconsBottom: {

    },
    receiver1: {
        marginRight: "58vw",
    },
    receiver2: {
        marginRight: "8vw"
    },
    receiver3: {
        marginRight: "58vw"
    },
    receiver4: {
        marginRight: "8vw"
    }
}));


function withMyHook(Component){
    return function WrappedComponent(props){
        const classes = useStyles();
        return <Component {...props} classes={classes}/>
    }
}

function Copyright() {
    return (
        <Typography variant="body2" color="textSecondary" align="center">
            {'Copyright © '}
            <Link color="inherit" href="https://github.com/mohaimenhasan/Indoor-Localization-Capstone-Project">
                ECE496 Capstone Project - group 2019100
            </Link>{' '}
            {new Date().getFullYear()}
            {'.'}
        </Typography>
    );
}


class RunSimulation extends Component{
    constructor(props){
        super(props);
        this.state = {
            open: true,
            currentMap: []
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

    changeToReport(event){
        this.props.appContext.setState({
            currentScreen: <AnalysisReport appContext={this.props.appContext}/>
        })
    }

    runSimulation = async (event) => {
        const classes = this.props.classes;
        this.setState({
           currentMap: <BorderLinearProgress
                           className={classes.margin}
                           variant="determinate"
                           color="secondary"
                           value={50}
                       />
        });
        await fetch(api_base_url+'analysis/getCurrentRun',
            {
                method: 'GET',
                headers: {
                    "Content-Type": "application/json",
                    "Access-Control-Allow-Origin": '*'
                }
            }).then(res => res.json())
            .then(response => {
                console.log(response);
                let r1x = [];
                let r1y = [];
                let r2x = [];
                let r2y = [];
                let r3x = [];
                let r3y = [];
                let r4x = [];
                let r4y = [];
                let heatMaps = [];
                let positionMatrix = response["position"][0];
                let xDim = response["griddim"][0][0];
                let yDim = response["griddim"][0][1];
                positionMatrix = Array.from(positionMatrix);

                for (let i=0; i < xDim; i=i+0.01){
                    let y_val = response["receivers"]["receiver1"]["line"]["slope"]*i+response["receivers"]["receiver1"]["line"]["intercept"]/100.0;
                    if (y_val >= 0 && y_val <= yDim){
                        r1x.push(i);
                        r1y.push(response["receivers"]["receiver1"]["line"]["slope"]*i+response["receivers"]["receiver1"]["line"]["intercept"]/100.0);
                    }
                }
                for (let i=0; i < xDim; i=i+0.01){
                    let y_val = response["receivers"]["receiver2"]["line"]["slope"]*i+response["receivers"]["receiver2"]["line"]["intercept"]/100.0;
                    if (y_val >= 0 && y_val <= yDim){
                        r2x.push(i);
                        r2y.push(response["receivers"]["receiver2"]["line"]["slope"]*i+response["receivers"]["receiver2"]["line"]["intercept"]/100.0);
                    }
                }

                for (let i=0; i < xDim; i=i+0.01){
                    let y_val = response["receivers"]["receiver3"]["line"]["slope"]*i+response["receivers"]["receiver3"]["line"]["intercept"]/100.0;
                    if (y_val >= 0 && y_val <= yDim){
                        r3x.push(i);
                        r3y.push(response["receivers"]["receiver3"]["line"]["slope"]*i+response["receivers"]["receiver3"]["line"]["intercept"]/100.0);
                    }
                }

                for (let i=0; i < xDim; i=i+0.01){
                    let y_val = response["receivers"]["receiver4"]["line"]["slope"]*i+response["receivers"]["receiver4"]["line"]["intercept"]/100.0;
                    if (y_val >= 0 && y_val <= yDim){
                        r4x.push(i);
                        r4y.push(response["receivers"]["receiver4"]["line"]["slope"]*i+response["receivers"]["receiver4"]["line"]["intercept"]/100.0);
                    }
                }

                let xLabels = [];
                let yLabels = [];

                for (let i=0; i < xDim; ++i){
                    xLabels.push(i);
                }

                for (let i=0; i < yDim; ++i){
                    yLabels.push(i);
                }
                const data = positionMatrix;

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
                                    x: [(response["receivers"]["receiver1"]["position"][0])/100.0],
                                    y: [(response["receivers"]["receiver1"]["position"][1])/100.0],
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
                                    x: [(response["receivers"]["receiver2"]["position"][0])/100.0],
                                    y: [(response["receivers"]["receiver2"]["position"][1])/100.0],
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
                                    x: [(response["receivers"]["receiver3"]["position"][0])/100.0],
                                    y: [(response["receivers"]["receiver3"]["position"][1])/100.0],
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
                                    x: [(response["receivers"]["receiver4"]["position"][0])/100.0],
                                    y: [(response["receivers"]["receiver4"]["position"][1])/100.0],
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
                                title: "Timeline: <b>"+response["timefrom"]+"</b> to <b>"+response["timeto"]+"</b>",
                                font: {
                                    size: "20px"
                                }}}
                        />
                    </div>
                );
                heatMaps.push(temp);


                this.setState({
                    currentMap: heatMaps
                });
            })
    };

    render() {
        let classes = this.props.classes;
        const fixedHeightPaper = clsx(classes.paper, classes.fixedHeight);
        const mainListItems = (
            <div>
                <ListItem button onClick={(event) => this.changeDashboard(event)}>
                    <ListItemIcon>
                        <DashboardIcon />
                    </ListItemIcon>
                    <ListItemText primary="Dashboard" />
                </ListItem>
                <ListItem button onClick={(event) => this.changeToReport(event)}>
                    <ListItemIcon>
                        <BarChartIcon />
                    </ListItemIcon>
                    <ListItemText primary="Past Results" />
                </ListItem>
                <ListItem button>
                    <ListItemIcon>
                        <PlayCircleOutlineIcon/>
                    </ListItemIcon>
                    <ListItemText primary="Run Simulation" />
                </ListItem>
            </div>
        );

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
                            Run Simulation
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
                        <Grid item xs={4} md={4} lg={4} style={{marginLeft: "25vw"}}>
                            <Paper className={fixedHeightPaper}>
                                <Button
                                    variant="contained"
                                    color="secondary"
                                    className={classes.button}
                                    startIcon={<PlayArrowIcon/>}
                                    onClick={(event)=> this.runSimulation(event)}
                                >
                                    Run Simulation
                                </Button>
                            </Paper>
                        </Grid>
                        <Grid item xs={12} md={12} lg={12}>
                            {this.state.currentMap}
                        </Grid>
                        <Box pt={4}>
                            <Copyright />
                        </Box>
                    </Container>
                </main>

            </div>
        )
    }
}

RunSimulation = withMyHook(RunSimulation);

export default RunSimulation;
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

let drawerWidth = 240;
let useStyles;

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
        height: 240,
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
            open: true
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

    render() {
        let classes = this.props.classes;
        let x = [];
        let y = [];
        for (let i=0; i < 100; ++i){
            x.push(i);
            y.push(3*i + 5);
        }

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
                            How It Works
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
                        <Grid item xs={12} md={12} lg={12}>
                            <Plot
                                data={[{
                                        z: [[1, 20, 30],
                                            [20, 1, 60],
                                            [30, 60, 1]],
                                        x: [1, 50, 100],
                                        y: [1, 50, 100],
                                        type: "heatmap",
                                        colorscale: "Portland"
                                    },
                                    {
                                        x: [2],
                                        y: [4.5],
                                        type: 'scatter',
                                        mode: 'markers',
                                        marker: {
                                            color: 'rgb(17, 157, 255)',
                                            size: 30,
                                            line: {
                                                color: 'rgb(255, 0, 255)',
                                                width: 6
                                            },
                                            symbol: 'asterisk'
                                        },
                                        showlegend: false,
                                        name: 'Legendary',
                                        hovertemplate: "Position: %{x}m,%{y}m"
                                    },{
                                        x: x,
                                        y: y,
                                        type: 'scatter',
                                        showlegend: false
                                    }]
                                }
                                layout={{
                                    width: "80vw",
                                    height: "60vh",
                                    title: "Heat Map of Position Estimation",
                                    font: {
                                        size: "20px"
                                    }}}
                            />
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
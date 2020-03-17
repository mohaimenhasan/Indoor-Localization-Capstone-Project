import React, {Component} from 'react';
import HeatMap from "react-simple-heatmap";
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
import TrackChangesIcon from '@material-ui/icons/TrackChanges';
import ChevronLeftIcon from "@material-ui/icons/ChevronLeft";
import RssFeedIcon from '@material-ui/icons/RssFeed';
import PositionData from "../sampleOut";
import AnalysisReport from "./AnalysisReport";


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


class currentRun extends Component{
    constructor(props){
        super(props);
        this.state = {
            open: false
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
        let positionMatrix = PositionData["position"];
        positionMatrix = Array.from(positionMatrix);
        const xLabels = new Array(positionMatrix.length).fill(0).map((_,i) => `${i}`);
        const xLabelsVisibility = new Array(24)
            .fill(0)
            .map((_, i) => (i %1 === 0));

        const yLabels = new Array(positionMatrix[0].length).fill(0).map((_,i) => `${i}`);

        const data = positionMatrix;
        for (let i in data){
            for (let j in data[i]){
                data[i][j] =Math.round((data[i][j]*100 + Number.EPSILON) * 1000) / 1000;
            }
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
                    <ListItemText primary="Reports" />
                </ListItem>
                <ListItem button>
                    <ListItemIcon>
                        <TrackChangesIcon />
                    </ListItemIcon>
                    <ListItemText primary="Changes" />
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
                            Current Run
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
                            <div className={classes.heatmapex}>
                                <div className={classes.iconsTop}>
                                        <IconButton className={classes.receiver1}>
                                        <RssFeedIcon fontSize={"large"}/>
                                    </IconButton>
                                    <IconButton className={classes.receiver2}>
                                        <RssFeedIcon fontSize={"large"}/>
                                    </IconButton>
                                </div>
                                    <HeatMap
                                        data={data}
                                        bgColors={ ["rgb(255, 11, 11)", "rgb(255, 255, 0)"] }
                                        xLabels={xLabels}
                                        yLabels={yLabels}
                                        xLabelsStyle={{ fontWeight: "bold", fontSize: "11px" }}
                                        yLabelsStyle={{ fontWeight: "bold" }}
                                        legendStyle={{ fontWeight: "bold" }}
                                        showLegend={ true }
                                        showData={true}
                                        xStepLabel={ 1 }
                                        yStepLabel={ 1 }
                                        showTicks={ "x" }
                                        bordered={ true }
                                        borderRadius={ "4px" }
                                        squares
                                        onClick={(data, x, y) => alert(`Data: ${ data }, X: ${x}, Y: ${y}`)}
                                    />
                                    <div className={classes.iconsBottom}>
                                        <IconButton className={classes.receiver3}>
                                            <RssFeedIcon fontSize={"large"}/>
                                        </IconButton>
                                        <IconButton className={classes.receiver4}>
                                            <RssFeedIcon fontSize={"large"}/>
                                        </IconButton>
                                    </div>
                            </div>
                        <Box pt={4}>
                            <Copyright />
                        </Box>
                    </Container>
                </main>
            </div>
        )
    }
}

currentRun = withMyHook(currentRun);

export default currentRun;
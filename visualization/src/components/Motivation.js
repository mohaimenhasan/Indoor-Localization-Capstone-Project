import React, {Component} from "react";
import mo from './mo.png';
import mo_cont from './mo_cont.png';
import gps from './gps.png';
import indoor from './indoor.png';
import work from './work.png';
import Current from "./HowItWorks";
import AnalysisReport from "./AnalysisReport";
import Typography from "@material-ui/core/Typography";
import Link from "@material-ui/core/Link";
import {makeStyles} from "@material-ui/core/styles";
import {red} from "@material-ui/core/colors";
import CssBaseline from "@material-ui/core/CssBaseline";
import AppBar from "@material-ui/core/AppBar";
import clsx from "clsx";
import Toolbar from "@material-ui/core/Toolbar";
import IconButton from "@material-ui/core/IconButton";
import MenuIcon from "@material-ui/icons/Menu";
import ChevronLeftIcon from "@material-ui/icons/ChevronLeft";
import Divider from "@material-ui/core/Divider";
import List from "@material-ui/core/List";
import Drawer from "@material-ui/core/Drawer";
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import DataUsageIcon from "@material-ui/icons/DataUsage";
import ListItemText from "@material-ui/core/ListItemText";
import BarChartIcon from "@material-ui/icons/BarChart";
import Dashboard from "./Dashboard";
import DashboardIcon from "@material-ui/icons/Dashboard";
import Box from "@material-ui/core/Box";
import PlayCircleOutlineIcon from "@material-ui/icons/PlayCircleOutline";

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
    media: {
        height: 0,
        paddingTop: '56.25%', // 16:9
    },
    expand: {
        transform: 'rotate(0deg)',
        marginLeft: 'auto',
        transition: theme.transitions.create('transform', {
            duration: theme.transitions.duration.shortest,
        }),
    },
    expandOpen: {
        transform: 'rotate(180deg)',
    },
    avatar: {
        backgroundColor: red[500],
    },
    button: {
        margin: theme.spacing(1),
    },
}));

function withMyHook(Component){
    return function WrappedComponent(props){
        const classes = useStyles();
        return <Component {...props} classes={classes}/>
    }
}

class Motivation extends Component{
    constructor(props) {
        super(props);
        this.state = {
            open: true
        }
    }

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

    changeToReport(event){
        this.props.appContext.setState({
            currentScreen: <AnalysisReport appContext={this.props.appContext}/>
        })
    }

    render(){
        const classes = this.props.classes;
        const handleDrawerOpen = () => {
            this.setState({
                open: true
            });
        };
        const handleDrawerClose = () => {
            this.setState({
                open: false
            })
        };

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
                            onClick={handleDrawerOpen}
                            className={clsx(classes.menuButton, this.state.open && classes.menuButtonHidden)}
                        >
                            <MenuIcon />
                        </IconButton>
                        <Typography component="h1" variant="h6" color="inherit" noWrap className={classes.title}>
                            Motivation
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
                        <IconButton onClick={handleDrawerClose}>
                            <ChevronLeftIcon />
                        </IconButton>
                    </div>
                    <Divider />
                    <List>{mainListItems}</List>
                    <Divider />
                </Drawer>
                <main className={classes.content}>
                    <div className={classes.appBarSpacer} />
                    <img src={mo} alt="mo" />
                    <img src={mo_cont} alt="mo_cont" />
                    <img src={gps} alt="gps" />
                    <img src={indoor} alt="indoor" />
                    <img src={work} alt="work" />
                </main>
                <Box pt={4}>
                    <Copyright />
                </Box>
            </div>
        );
    }
}

Motivation = withMyHook(Motivation);
export default Motivation
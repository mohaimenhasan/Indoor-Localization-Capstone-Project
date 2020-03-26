import React, {Component} from 'react';
import clsx from 'clsx';
import { makeStyles } from '@material-ui/core/styles';
import CssBaseline from '@material-ui/core/CssBaseline';
import Drawer from '@material-ui/core/Drawer';
import Box from '@material-ui/core/Box';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import List from '@material-ui/core/List';
import Typography from '@material-ui/core/Typography';
import Divider from '@material-ui/core/Divider';
import IconButton from '@material-ui/core/IconButton';
import Container from '@material-ui/core/Container';
import Grid from '@material-ui/core/Grid';
import Paper from '@material-ui/core/Paper';
import Link from '@material-ui/core/Link';
import MenuIcon from '@material-ui/icons/Menu';
import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import BarChartIcon from '@material-ui/icons/BarChart';
import DataUsageIcon from '@material-ui/icons/DataUsage';
import Current from "./HowItWorks";
import FloorPlan from './markedFloor.jpg';
import AnalysisReport from "./AnalysisReport";
import Card from "@material-ui/core/Card";
import CardContent from "@material-ui/core/CardContent";
import CardActions from "@material-ui/core/CardActions";
import {red} from "@material-ui/core/colors";
import MapIcon from '@material-ui/icons/Map';
import Button from "@material-ui/core/Button";
import EmojiEmotionsIcon from '@material-ui/icons/EmojiEmotions';
import BuildIcon from '@material-ui/icons/Build';
import Motivation from './Motivation';
import Setup from "./Setup";
import PlayCircleOutlineIcon from '@material-ui/icons/PlayCircleOutline';
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
    fixedFunct:{
      height: 350
    },
    fixedFloorHeight:{
        height: 460
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

class Dashboard extends Component{
    constructor(props){
        super(props);
        this.state = {
            open: true,
            expanded: false,
            showFloor: "None"
        }
    }

    changeToMotivation(event){
        this.props.appContext.setState({
            currentScreen: <Motivation appContext={this.props.appContext}/>
        })
    }

    changeToSetup(event){
        this.props.appContext.setState({
            currentScreen: <Setup appContext={this.props.appContext}/>
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

    changeToReport(event){
        this.props.appContext.setState({
            currentScreen: <AnalysisReport appContext={this.props.appContext}/>
        })
    }

    handleExpandClick = (event) => {
        if (this.state.showFloor === "None"){
            this.setState({
                showFloor: ""
            })
        }else{
            this.setState({
                showFloor: "None"
            })
        }
    };


    render() {
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
        const fixedHeightPaper = clsx(classes.paper, classes.fixedHeight);
        const fixedFloorPlan = clsx(classes.paper, classes.fixedFloorHeight);
        const fixedFuncHeight = clsx(classes.paper, classes.fixedFunct);

        const mainListItems = (
            <div>
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
                <ListItem button onClick={(event) => this.changeToRun(event)}>
                    <ListItemIcon>
                        <PlayCircleOutlineIcon/>
                    </ListItemIcon>
                    <ListItemText primary="Run Simulation" />
                </ListItem>
            </div>
        );

        return (
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
                            Dashboard
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
                    <Container maxWidth="lg" className={classes.container}>
                        <Grid container spacing={3}>
                            <Grid item xs={6.5} md={6.5} lg={6.5}>
                                <Paper className={fixedHeightPaper}>
                                    <iframe
                                        title={"Bahen Map"}
                                        width={"500"}
                                        height={"500"}
                                        id={"gmap_canvas"}
                                        src={"https://maps.google.com/maps?q=bahen&t=&z=17&ie=UTF8&iwloc=&output=embed"}
                                        frameBorder={"0"}
                                        scrolling={"no"}
                                        marginHeight={"0"}
                                        marginWidth={"0"}>
                                    </iframe>
                                </Paper>
                            </Grid>
                            <Grid item xs={6.5} md={5} lg={5}>
                                <Card className={classes.root}>
                                    <CardContent>
                                        <Typography variant="h4" color="textPrimary" component="p">
                                            Indoor Localization Capstone Project
                                        </Typography>
                                        <br/>
                                        <Typography variant="h5" color="textSecondary" component="p">
                                           <b>Group 2019100</b>
                                        </Typography>
                                        <Typography variant="h6" color="textSecondary" component="p">
                                            {new Date().toLocaleTimeString()}
                                        </Typography>
                                        <Typography variant="p" color="secondary" component="p">
                                            <br/>
                                            <b> Click on the map icon to see floor plan </b>
                                        </Typography>
                                    </CardContent>
                                    <CardActions disableSpacing>
                                        <IconButton
                                            color="secondary"
                                            className={clsx(classes.expand, {
                                                [classes.expandOpen]: this.state.expanded,
                                            })}
                                            onClick={this.handleExpandClick}
                                            aria-expanded={this.state.expanded}
                                            aria-label="show more"
                                        >
                                            <MapIcon/>
                                        </IconButton>
                                    </CardActions>
                                </Card>
                            </Grid>
                            <Grid item xs={12} md={4} lg={4}>
                                <Paper className={fixedFuncHeight}>
                                    <Typography variant="h4" color="textPrimary" component="p">
                                        Main Menu
                                        <br/> <br/>
                                    </Typography>
                                    <Button
                                        variant="contained"
                                        color="secondary"
                                        className={classes.button}
                                        startIcon={<EmojiEmotionsIcon/>}
                                        onClick={(event)=>{this.changeToMotivation(event)}}
                                    >
                                        Motivation
                                    </Button>
                                    <Button
                                        variant="contained"
                                        color="primary"
                                        className={classes.button}
                                        startIcon={<BuildIcon/>}
                                        onClick={(event)=> this.changeToSetup(event)}
                                    >
                                        Setup
                                    </Button>
                                    <Button
                                        variant="contained"
                                        color="primary"
                                        className={classes.button}
                                        startIcon={<DataUsageIcon/>}
                                        onClick={(event) => this.changeToCurrent(event)}
                                    >
                                        How it works
                                    </Button>
                                    <Button
                                        variant="contained"
                                        color="primary"
                                        className={classes.button}
                                        startIcon={<BarChartIcon/>}
                                        onClick={(event) => this.changeToReport(event)}
                                    >
                                        Past Results
                                    </Button>
                                </Paper>
                            </Grid>
                            <Grid
                                item xs={12} md={7} lg={7}
                            >
                                <Paper className={fixedFloorPlan} style={{
                                    display: this.state.showFloor,
                                    border: "5px inset #00A417"
                                }}>
                                    <img src={FloorPlan} alt="floorplan" />;
                                </Paper>
                            </Grid>
                        </Grid>
                        <Box pt={4}>
                            <Copyright />
                        </Box>
                    </Container>
                </main>
            </div>
        );
    }
}

Dashboard = withMyHook(Dashboard);

export default Dashboard;

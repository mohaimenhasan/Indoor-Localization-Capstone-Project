import React, {Component} from "react";
import Typography from "@material-ui/core/Typography";
import Link from "@material-ui/core/Link";
import {makeStyles} from "@material-ui/core/styles";
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import DashboardIcon from "@material-ui/icons/Dashboard";
import ListItemText from "@material-ui/core/ListItemText";
import DataUsageIcon from "@material-ui/icons/DataUsage";
import BarChartIcon from "@material-ui/icons/BarChart";
import Dashboard from "./Dashboard";
import AnalysisReport from "./AnalysisReport";
import {CssBaseline} from "@material-ui/core";
import AppBar from "@material-ui/core/AppBar";
import clsx from "clsx";
import Toolbar from "@material-ui/core/Toolbar";
import IconButton from "@material-ui/core/IconButton";
import MenuIcon from "@material-ui/icons/Menu";
import Drawer from "@material-ui/core/Drawer";
import ChevronLeftIcon from "@material-ui/icons/ChevronLeft";
import Divider from "@material-ui/core/Divider";
import List from "@material-ui/core/List";
import Container from "@material-ui/core/Container";
import Grid from "@material-ui/core/Grid";
import Paper from "@material-ui/core/Paper";
import Box from "@material-ui/core/Box";
import justsdr from "./sdrcomplete.png";
import transmission from "./transmission.png";
import VisibilityIcon from '@material-ui/icons/Visibility';
import VisibilityOffIcon from '@material-ui/icons/VisibilityOff';
import Setup from './Setup';

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
        height: 445
    },
    fixedHeight: {
        height: 90,
    },
    fixedSetupHeight: {
        height: 610
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

    },
    button: {
        margin: theme.spacing(1),
    },
    cardRoot: {
        textAlign: "left",
        border: "5px inset #00A417",
        maxWidth: 345
    },
    allButtons:{
        marginLeft: "17vw",
        margin: theme.spacing(2)
    },
    dot: {
        height: "50px",
        width: "50px",
        backgroundColor: "#red",
        borderRadius: "50%",
        display: "inline-block"
    }
}));

function withMyHook(Component){
    return function WrappedComponent(props){
        const classes = useStyles();
        return <Component {...props} classes={classes}/>
    }
}

class HowItWorks extends Component{
    constructor(props) {
        super(props);
        this.state = {
            open: true,
            opensdr: false,
            opensrv: false,
            checkedA: false,
            showComponentsText: "Show Components",
            displayComponents: "None",
            showComponentIcon: <VisibilityIcon/>,
            showSetupText: "Hide Material Setup",
            displaySetup: "",
            showSetupIcon: <VisibilityOffIcon/>,
            displayImage: "None",
            wave1: "None",
            wave2: "None",
            wave3: "None"
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
            currentScreen: <Setup appContext={this.props.appContext}/>
        });
    }

    changeToReport(event){
        this.props.appContext.setState({
            currentScreen: <AnalysisReport appContext={this.props.appContext}/>
        })
    }

    componentDidMount() {
        this.setState({
            checkedA: !this.state.checkedA
        })
    }

    componentDidUpdate() {
        let cur = this;
        if (this.state.wave1 === "None" && this.state.wave2 === "None" && this.state.wave3 === "None"){
            setTimeout(function () {
                cur.setState({
                    wave1: "",
                })
            }, 200)
        }
        if (this.state.wave1 === "" && this.state.wave2 === "None" && this.state.wave3 === "None"){
            setTimeout(function () {
                cur.setState({
                    wave2: ""
                })
            }, 800)
        }
        if (this.state.wave1 === "" && this.state.wave2 === "" && this.state.wave3 === "None"){
            setTimeout(function () {
                cur.setState({
                    wave3: ""
                })
            }, 800)
        }
        if (this.state.wave1 === "" && this.state.wave2 === "" && this.state.wave3 === ""){
            setTimeout(function () {
                cur.setState({
                    wave1: "None",
                    wave2: "None",
                    wave3: "None"
                })
            }, 800)
        }
    }

    render() {

        const classes = this.props.classes;
        const fixedSetupPaper = clsx(classes.paper, classes.fixedSetupHeight);
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
                    <ListItemText primary="Setup" />
                </ListItem>
                <ListItem button onClick={(event) => this.changeToReport(event)}>
                    <ListItemIcon>
                        <BarChartIcon />
                    </ListItemIcon>
                    <ListItemText primary="Past Results" />
                </ListItem>
            </div>
        );

        return(
            <div className={classes.root}>
                <CssBaseline/>
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
                            How the Setup Works
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
                        <Grid container spacing={2}>
                            <Grid item xs={12} md={12} lg={12} style={{display: this.state.displaySetup, border: "5px inset #A40000"}}>
                                <Paper className={fixedSetupPaper} style={{
                                    backgroundSize: "40px 40px",
                                    backgroundImage: "linear-gradient(to right, grey 1px, transparent 1px),linear-gradient(to bottom, grey 1px, transparent 1px)"
                                }}>
                                    <Typography variant="h6" style={{color: "green"}} component="p"><b>Bahen Wireless Lab: 4145</b></Typography>
                                    <img
                                        src={justsdr}
                                        alt={"receiver-1"}
                                        style={{
                                            height: "10vh",
                                            width: "10vw"
                                        }}
                                    />
                                    <img
                                        src={justsdr}
                                        alt={"receiver-2"}
                                        style={{
                                            height: "10vh",
                                            width: "10vw",
                                            marginLeft: "60vw",
                                            marginTop: "-10vh"
                                        }}
                                    />
                                    <img
                                        src={justsdr}
                                        alt={"transmitter"}
                                        style={{
                                            marginTop: "15vh",
                                            marginLeft: "30vw",
                                            height: "10vh",
                                            width: "10vw"
                                        }}
                                    />
                                    <img
                                        src={justsdr}
                                        alt={"receiver-3"}
                                        style={{
                                            marginTop: "10vh",
                                            height: "10vh",
                                            width: "10vw"
                                        }}
                                    />
                                    <img
                                        src={justsdr}
                                        alt={"receiver-4"}
                                        style={{
                                            marginTop: "-10vh",
                                            marginLeft: "60vw",
                                            height: "10vh",
                                            width: "10vw"
                                        }}
                                    />
                                    <h4 style={{
                                        height: "10vh",
                                        width: "10vw",
                                        marginTop: "-57vh",
                                        color: "red"
                                    }}> Receiver-1 </h4>
                                    <h4 style={{
                                        height: "10vh",
                                        width: "10vw",
                                        marginLeft: "60vw",
                                        marginTop: "0vh",
                                        color: "red"
                                    }}> Receiver-2 </h4>
                                    <h4 style={{
                                        height: "10vh",
                                        width: "10vw",
                                        marginLeft: "30vw",
                                        marginTop: "18vh",
                                        color: "green"
                                    }}> Transmitter </h4>
                                    <h4 style={{
                                        height: "10vh",
                                        width: "10vw",
                                        color: "red"
                                    }}> Receiver-3 </h4>
                                    <h4 style={{
                                        height: "10vh",
                                        width: "10vw",
                                        marginTop: "-25vh",
                                        marginLeft: "60vw",
                                        color: "red"
                                    }}> Receiver-4 </h4>
                                    <img
                                        src={transmission}
                                        alt={"transmission"}
                                        style={{
                                            height: "20vh",
                                            width: "20vw",
                                            marginTop: "-35vh",
                                            marginLeft: "25vw",
                                            display: this.state.wave1
                                        }}
                                    />
                                    <img
                                        src={transmission}
                                        alt={"transmission"}
                                        style={{
                                            height: "25vh",
                                            width: "25vw",
                                            marginTop: "-23vh",
                                            marginLeft: "23vw",
                                            display: this.state.wave2
                                        }}
                                    />
                                    <img
                                        src={transmission}
                                        alt={"transmission"}
                                        style={{
                                            height: "30vh",
                                            width: "30vw",
                                            marginTop: "-28vh",
                                            marginLeft: "20vw",
                                            display: this.state.wave3
                                        }}
                                    />
                                </Paper>
                            </Grid>
                            <Grid item xs={12} md={12} lg={12} style={{display: this.state.displayImage, border: "5px inset #A40000"}}>

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

HowItWorks = withMyHook(HowItWorks);
export default HowItWorks;
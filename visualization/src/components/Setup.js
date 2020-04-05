import React, {Component} from "react";
import Typography from "@material-ui/core/Typography";
import Link from "@material-ui/core/Link";
import {makeStyles} from "@material-ui/core/styles";
import {red, yellow} from "@material-ui/core/colors";
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import DashboardIcon from "@material-ui/icons/Dashboard";
import ListItemText from "@material-ui/core/ListItemText";
import DataUsageIcon from "@material-ui/icons/DataUsage";
import BarChartIcon from "@material-ui/icons/BarChart";
import Dashboard from "./Dashboard";
import Current from "./HowItWorks";
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
import Card from "@material-ui/core/Card";
import CardContent from "@material-ui/core/CardContent";
import CardActions from "@material-ui/core/CardActions";
import MoreVertIcon from '@material-ui/icons/MoreVert';
import CardHeader from "@material-ui/core/CardHeader";
import Avatar from "@material-ui/core/Avatar";
import SDR from "./sdrboard.gif";
import ExpressAPI from "./expressapi.png";
import CardMedia from "@material-ui/core/CardMedia";
import ExpandMoreIcon from '@material-ui/icons/ExpandMore';
import Box from "@material-ui/core/Box";
import Collapse from "@material-ui/core/Collapse";
import Button from "@material-ui/core/Button";
import justsdr from "./sdrcomplete.png";
import transmission from "./transmission.png";
import VisibilityIcon from '@material-ui/icons/Visibility';
import VisibilityOffIcon from '@material-ui/icons/VisibilityOff';
import {Pannellum} from "pannellum-react";
import Panorama from "./panaroma.jpg";

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
        height: 500
    },
    fixedFloorHeight:{
        height: 445
    },
    fixedHeight: {
        height: 200,
    },
    fixedSetupHeight: {
        height: 660
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
    },
    line: {
        height: "47px",
        borderBottom: "5px dotted red",
    },
    vertical: {
        width: "0px",
        height: "100%",
        borderLeft: "5px dotted red",
    }
}));

function withMyHook(Component){
    return function WrappedComponent(props){
        const classes = useStyles();
        return <Component {...props} classes={classes}/>
    }
}

class Setup extends Component{
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
            currentScreen: <Current appContext={this.props.appContext}/>
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


    render() {

        const classes = this.props.classes;
        const fixedHeightPaper = clsx(classes.paper, classes.fixedHeight);
        const fixedSetupPaper = clsx(classes.paper, classes.fixedSetupHeight);
        const fixedFuncHeight = clsx(classes.paper, classes.fixedFunct);
        const handleDrawerOpen = () => {
            this.setState({
                open: true
            });
        };

        const showComponents = () => {
            if (this.state.displayComponents === "None"){
                this.setState({
                    displayComponents: "",
                    showComponentsText: "Hide Components",
                    showComponentIcon: <VisibilityOffIcon/>
                })
            }else {
                this.setState({
                    displayComponents: "None",
                    showComponentsText: "Show Components",
                    showComponentIcon: <VisibilityIcon/>
                })
            }

        };

        const showSetup = () => {
            if (this.state.displaySetup === "None"){
                this.setState({
                    showSetupText: "Hide Material Setup",
                    displaySetup: "",
                    showSetupIcon: <VisibilityOffIcon/>
                })
            }else{
                this.setState({
                    showSetupText: "Show Material Setup",
                    displaySetup: "None",
                    showSetupIcon: <VisibilityIcon/>
                })
            }
        };

        const handleDrawerClose = () => {
            this.setState({
                open: false
            })
        };

        const handleSDRClick = () => {
            this.setState({
                opensdr: !this.state.opensdr
            })
        };

        const handleSrvClick = () => {
            this.setState({
                opensrv: !this.state.opensrv
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
                            Setup
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
                            <Grid item xs={12} md={7} lg={7} className={classes.allButtons}>
                                <Paper className={fixedHeightPaper}>
                                    <Button
                                        variant="outlined"
                                        color="primary"
                                        className={classes.button}
                                        startIcon={this.state.showComponentIcon}
                                        onClick={showComponents}
                                    >
                                        <b>{this.state.showComponentsText}</b>
                                    </Button>
                                    <Button
                                        variant="outlined"
                                        color="primary"
                                        className={classes.button}
                                        startIcon={this.state.showSetupIcon}
                                        onClick={showSetup}
                                    >
                                        <b>{this.state.showSetupText}</b>
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
                                </Paper>
                            </Grid>
                            <Grid item xs={12} md={5} lg={5}>
                                <Card className={classes.cardRoot} style={{display: this.state.displayComponents}}>
                                    <CardHeader
                                        avatar={
                                            <Avatar aria-label="recipe" style={{backgroundColor: yellow[500]}} className={classes.avatar}>
                                                <b>L</b>
                                            </Avatar>
                                        }
                                        action={
                                            <IconButton aria-label="settings">
                                                <MoreVertIcon />
                                            </IconButton>
                                        }
                                        title="LimeSDR - SDR"
                                        subheader="Lime Microsystems"
                                    />
                                    <CardMedia
                                        className={classes.media}
                                        image={SDR}
                                        title="SDR Board"
                                    />
                                    <CardContent>
                                        <Typography variant="body2" color="textPrimary" component="p">
                                            <b> SDR: </b>Software Defined Radio has been used to transmit and receive custom wifi packets
                                            generated using GNU radio.
                                        </Typography>
                                    </CardContent>
                                    <CardActions disableSpacing>
                                        <IconButton
                                            className={clsx(classes.expand, {
                                                [classes.expandOpen]: this.state.opensdr,
                                            })}
                                            onClick={handleSDRClick}
                                            aria-expanded={this.state.opensdr}
                                            aria-label="show more"
                                        >
                                            <ExpandMoreIcon />
                                        </IconButton>
                                    </CardActions>
                                    <Collapse in={this.state.opensdr} timeout="auto" unmountOnExit>
                                        <CardContent>
                                            <Typography variant="h8" color="textPrimary" component="p"><b>Objectives & Requirements:</b></Typography>
                                            <Typography variant="body2" color="textPrimary" component="p">
                                                <br/>
                                                <li> Software Defined Radio (SDR) – Must have at least 3 APs set up using SDR </li>
                                                <li> Each SDR should have at least 2 antennas. The number of antennas per SDR should increase for additional signals. </li>
                                                <br/>
                                            </Typography>
                                            <Typography variant="h8" color="textPrimary" component="p"><b>Description:</b></Typography>
                                            <Typography variant="body2" color="textPrimary" component="p">
                                                <br/>
                                                This module collects data from an input transmitter. Using GNU radio, an
                                                opensource software-defined radio system, transmitting packets are generated.
                                                Using the two antennas between the receiver, any phase change is detected
                                                as the object changes positions.
                                            </Typography>
                                        </CardContent>
                                    </Collapse>
                                </Card>
                            </Grid>
                            <Grid item xs={12} md={5} lg={5}>
                                <Card className={classes.cardRoot} style={{display: this.state.displayComponents}}>
                                    <CardHeader
                                        avatar={
                                            <Avatar aria-label="recipe" style={{backgroundColor: red[500]}} className={classes.avatar}>
                                                <b>S</b>
                                            </Avatar>
                                        }
                                        action={
                                            <IconButton aria-label="settings">
                                                <MoreVertIcon />
                                            </IconButton>
                                        }
                                        title="Data Server"
                                        subheader="ExpressJS API"
                                    />
                                    <CardMedia
                                        className={classes.media}
                                        image={ExpressAPI}
                                        title="Express API"
                                    />
                                    <CardContent>
                                        <Typography variant="body2" color="textPrimary" component="p">
                                            <b> Processing Server: </b> Processing Server to communicate with the database and the frontend dashbaord in real time
                                        </Typography>
                                    </CardContent>
                                    <CardActions disableSpacing>
                                        <IconButton
                                            className={clsx(classes.expand, {
                                                [classes.expandOpen]: this.state.opensrv,
                                            })}
                                            onClick={handleSrvClick}
                                            aria-expanded={this.state.opensrv}
                                            aria-label="show more"
                                        >
                                            <ExpandMoreIcon />
                                        </IconButton>
                                    </CardActions>
                                    <Collapse in={this.state.opensrv} timeout="auto" unmountOnExit>
                                        <CardContent>
                                            <Typography variant="h8" color="textPrimary" component="p"><b>Objectives & Requirements:</b></Typography>
                                            <Typography variant="body2" color="textPrimary" component="p">
                                                <br/>
                                                <li> The system should be accurately able to measure the change of angles, along with the location, with a change of position  </li>
                                                <li> The system must be able to collect Channel-State-Information data without modification of any existing hardware  </li>
                                                <li> Each receiver is connected to a computer which needs to be connected within the same local network as the server. </li>
                                                <li> Using POST request, each computer can publish data on every receiver to the central processing server </li>
                                                <br/>
                                            </Typography>
                                            <Typography variant="h8" color="textPrimary" component="p"><b>Description:</b></Typography>
                                            <Typography variant="body2" color="textPrimary" component="p">
                                                <br/>
                                                Central processing server handles all the API calls. After the computers
                                                publish the data into the processing server through POST call, the central
                                                processing server synchronizes the data from all the receivers. Once all the
                                                data is processed, the central server stores the data in the database as a
                                                record for past data processing purposes. Central processing server estimates
                                                the phase changes through the data and provides measurements to the frontend
                                                for visually estimating the changes.
                                            </Typography>
                                        </CardContent>
                                    </Collapse>
                                </Card>
                            </Grid>
                            <Grid item xs={12} md={12} lg={12} style={{display: this.state.displaySetup, border: "5px inset #A40000"}}>
                                <Paper className={fixedSetupPaper} style={{
                                    backgroundSize: "40px 40px",
                                    backgroundImage: "linear-gradient(to right, grey 1px, transparent 1px),linear-gradient(to bottom, grey 1px, transparent 1px)"
                                }}>
                                    <div className={classes.line}>
                                        <h4 style={{color: "red"}}><b> 5 meter </b></h4>
                                    </div>
                                    <Typography variant="h6" style={{color: "blue"}} component="p"><b>Bahen Wireless Lab: 4145</b></Typography>
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
                            <Grid item xs={12} md={12} lg={12} style={{display: this.state.displaySetup, border: "5px inset #A40000"}}>
                                <Paper className={fixedFuncHeight}>
                                    <Pannellum
                                        width="100%"
                                        height="500px"
                                        image={Panorama}
                                        pitch={10}
                                        yaw={180}
                                        hfov={110}
                                        autoLoad
                                        showZoomCtrl={false}
                                    >
                                        <Pannellum.Hotspot
                                            type="custom"
                                            pitch={31}
                                            yaw={150}
                                            handleClick={(evt, name) => console.log(name)}
                                            name="hs1"
                                        />
                                    </Pannellum>
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

Setup = withMyHook(Setup);
export default Setup;
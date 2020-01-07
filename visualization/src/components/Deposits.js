import React from 'react';
import Link from '@material-ui/core/Link';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import Title from './Title';

function preventDefault(event) {
    event.preventDefault();
}

const useStyles = makeStyles({
    depositContext: {
        flex: 1,
    },
});

export default function Deposits() {
    const classes = useStyles();
    let today = new Date();
    return (
        <React.Fragment>
            <Title>Recent Results</Title>
            <Typography component="p" variant="h4">
                90%
            </Typography>
            <Typography component="p" variant="h5">
                Accuracy
            </Typography>
            <Typography color="textSecondary" className={classes.depositContext}>
                {today.getDate()}/{today.getMonth()}/{today.getFullYear()}
            </Typography>
            <div>
                <Link color="primary" href="#" onClick={preventDefault}>
                    View results
                </Link>
            </div>
        </React.Fragment>
    );
}
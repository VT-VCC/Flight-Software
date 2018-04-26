const express = require('express');
const mysql = require('mysql');

// MySQL DB Credentials www.cubesat.aoe.vt.edu

// Create connection
// const db = mysql.createConnection({
//     host : 'database.hosting.vt.edu',
//     user : 'site2105',
//     password : 'BODAUNTU',
//     database : 'database2105'

// });

//local host connection

//Create connection
const db = mysql.createConnection({
    host : 'localhost',
    user : 'root',
    password : 'Password123',
    database : 'nodemysql'

});

// Connect
db.connect((err) => {
    if (err) throw err;

    console.log('MySQL Connected');
})

const app = express();

// create table
app.get('/createpoststable', (req, res) => {
    let sql = 'CREATE TABLE posts(id int AUTO_INCREMENT, title VARCHAR(255), body VARCHAR(255), PRIMARY KEY(id))'
    db.query(sql, (err, result) => {
        if(err) throw err;
        console.log(result);
        res.send('Post table created...');
    });
});

// Insert Post 1
app.get('/addpost1', (req, res) => {
    let post = {
        title: 'Post One',
        body : 'This is post number one'
    };
    let sql = 'INSERT INTO posts SET ?';
    let query = db.query(sql, post, (err, result) => {
        if(err) throw err;
        console.log(result);
        res.send('Post 1 inserted...');
    });
});

// Insert Post
app.get('/addpost/:title/:body', (req, res) => {
    let post = {
        title: req.params.title,
        body : req.params.body
    };
    let sql = 'INSERT INTO posts SET ?';
    let query = db.query(sql, post, (err, result) => {
        if(err) throw err;
        console.log(result);
        res.send(req.params.title + ' inserted...');
    });
});

// Select Posts
app.get('/getposts', (req, res) => {
    let sql = 'SELECT * FROM posts';
    let query = db.query(sql, (err, results) => {
        if(err) throw err;
        console.log(results);
        res.send('Posts Fetched...');
    });
});

// Select Single Post
app.get('/getpost/:id', (req, res) => {
    let sql =`SELECT * FROM posts WHERE id = ${req.params.id}`;
    let query = db.query(sql, (err, result) => {
        if(err) throw err;
        console.log(result);
        res.send('Posts Fetched...');
    });
});

app.listen('3000', () => {
    console.log('Server started on port 3000');
});


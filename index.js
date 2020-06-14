const basicAuth = require("express-basic-auth");
const bodyParser = require("body-parser");
const cors = require("cors");
const dotenv = require("dotenv");
const express = require("express");
const fs = require("fs");

dotenv.config();
const app = express();
const port = 3001;

app.use(cors());
app.use(bodyParser.raw({ limit: "500kb" }));
app.use(express.static("public"));

const user = process.env.API_USER;
const pwd = process.env.API_USER_PWD;

console.log(user, pwd);

app.post("/api/images", basicAuth({ users: { [user]: pwd } }), (req, res) => {
  const timestamp = req.get("Picture-FileName");
  if (!fs.exists("./public/images", () => {}))
    fs.mkdir("./public/images", () => {});
  fs.writeFile(`./public/images/${timestamp}.jpg`, req.body, (err) => {
    if (err) return console.log(err);
  });
  res.sendStatus(200);
});

app.get("/api/images", (req, res) => {
  if (!fs.exists("./public/images", () => {}))
    fs.mkdir("./public/images", () => {});
  const files = fs.readdirSync("./public/images");
  res.send({ ids: files });
});

app.listen(port, () => {});

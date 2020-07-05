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
app.use(bodyParser.raw({ limit: "50mb" }));
app.use(express.static("public"));

const user = process.env.API_USER;
const pwd = process.env.API_USER_PWD;

console.log(user, pwd);

app.post("/api/images", basicAuth({ users: { [user]: pwd } }), (req, res) => {
  const sender = req.get("Picture-FileName");
  if (!fs.existsSync("./public/images"))
    fs.mkdir("./public/images", (err) => {
      console.error(err);
    });
  //console.log(req);
  fs.writeFile(
    `./public/images/${sender} ${Date.now()}.jpg`,
    req.body,
    (err) => {
      if (err) return console.log(err);
    }
  );
  res.sendStatus(200);
});

app.get("/api/images", (req, res) => {
  if (!fs.existsSync("./public/images")) res.send({ ids: [] });
  else {
    const files = fs.readdirSync("./public/images");
    res.send({
      ids: files
        .sort((first, second) => {
          first = first
            .split(" ")
            .slice(1)
            .join(" ")
            .split(".")
            .slice(0, -1)
            .join(" ");
          second = second
            .split(" ")
            .slice(1)
            .join(" ")
            .split(".")
            .slice(0, -1)
            .join(" ");
          return first - second;
        })
        .reverse(),
    });
  }
});

app.listen(port, () => {});

const basicAuth = require("express-basic-auth");
const bodyParser = require("body-parser");
const cors = require("cors");
const dotenv = require("dotenv");
const express = require("express");
const fs = require("fs");
const sharp = require("sharp");

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
  if (!fs.existsSync("./public/images/previews"))
    fs.mkdir("./public/images/previews", (err) => {
      console.error(err);
    });

  const timestamp = Date.now();

  fs.writeFile(
    `./public/images/${sender} ${timestamp}.jpg`,
    req.body,
    (err) => {
      if (err) return console.log(err);
    }
  );

  sharp(req.body)
    .resize({ width: 720 })
    .flop()
    .flip()
    .toFile(`./public/images/previews/${sender} ${timestamp}.jpg`)
    .then((info) => {
      console.log(info);
    })
    .catch((err) => {
      console.log(err);
    });

  res.sendStatus(200);
});

app.get("/api/images", (req, res) => {
  if (!fs.existsSync("./public/images")) res.send({ ids: [] });
  else {
    let files = fs.readdirSync("./public/images");
    files = files.filter((file) => {
      try {
        const dirEntry = fs.statSync(`./public/images/${file}`);
        return dirEntry.isFile();
      } catch (error) {
        console.log("Failed to check if file: " + error);
        return false;
      }
    });
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

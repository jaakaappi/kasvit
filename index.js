const bodyParser = require("body-parser");
const express = require("express");
const fs = require("fs");

const app = express();
const port = 3000;

app.use(bodyParser.raw({ limit: "500kb" }));

app.post("/images", (req, res) => {
  const timestamp = req.get("Picture-FileName");
  if (!fs.exists("/images", () => {})) fs.mkdir("./images", () => {});
  fs.writeFile(`./images/${timestamp}.jpg`, req.body, (err) => {
    if (err) return console.log(err);
  });
  res.sendStatus(200);
});

app.get("/images", (req, res) => {
  if (!fs.exists("/images", () => {})) fs.mkdir("./images", () => {});
  const files = fs.readdirSync("./images");
  console.log(files);
  res.send({ ids: files });
});

app.listen(port, () => {});

const bodyParser = require("body-parser");
const cors = require("cors");
const express = require("express");
const fs = require("fs");

const app = express();
const port = 3001;

app.use(cors());
app.use(bodyParser.raw({ limit: "500kb" }));
app.use(express.static("public"));

app.post("/api/images", (req, res) => {
  const timestamp = req.get("Picture-FileName");
  if (!fs.exists("/images", () => {})) fs.mkdir("./images", () => {});
  fs.writeFile(`./images/${timestamp}.jpg`, req.body, (err) => {
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

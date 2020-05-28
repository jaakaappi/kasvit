const express = require("express");
const app = express();
const port = 3000;

app.post("/images", (req, res) => {
  res.send({
    update: true,
  });
});

app.listen(port, () => {});

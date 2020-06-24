import axios from "axios";
import React, { useEffect, useState } from "react";

function App() {
  const [imagesNames, setImageNames] = useState([]);
  useEffect(() => {
    axios
      .get(`${process.env.REACT_APP_API_URL}/api/images`)
      .then((response) => {
        let images = [];
        response.data.ids.map((id) => {
          images.push({
            filename: id,
            datetime: new Date(parseInt(id.split(" ")[1].split(".")[0])),
            sender: id.split(" ")[0],
          });
        });
        setImageNames(images);
      })
      .catch((error) => {
        console.error(error);
      });
  }, []);
  return (
    <div>
      {imagesNames.map((imageObject) => {
        return (
          <div key={imageObject.filename}>
            <p>{`${imageObject.sender} ${imageObject.datetime.toLocaleString(
              "en-GB"
            )}`}</p>
            <img
              style={{ width: "720px" }}
              src={`${process.env.REACT_APP_API_URL}/images/${imageObject.filename}`}
              alt={imageObject.filename}
            />
          </div>
        );
      })}
    </div>
  );
}

export default App;

import axios from "axios";
import React, { useEffect, useState } from "react";

function App() {
  const [imagesNames, setImageNames] = useState([]);
  useEffect(() => {
    axios
      .get(`${process.env.REACT_APP_API_URL}/api/images`)
      .then((response) => {
        setImageNames(response.data.ids);
      })
      .catch((error) => {
        console.error(error);
      });
  }, []);
  return (
    <div>
      {imagesNames.map((image) => {
        return (
          <div key={image}>
            <p>{image}</p>
            <img
              style={{ width: "720px" }}
              src={`${process.env.REACT_APP_API_URL}/images/${image}`}
              alt={image}
            />
          </div>
        );
      })}
    </div>
  );
}

export default App;

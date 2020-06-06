import axios from "axios";
import React, { useEffect, useState } from "react";

function App() {
  const [imagesNames, setImageNames] = useState([]);
  useEffect(() => {
    axios
      .get("http://localhost:3001/api/images")
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
            <img src={`http://localhost:3001/images/${image}`} alt={image} />
          </div>
        );
      })}
    </div>
  );
}

export default App;

let port : number = Number(document.location.port);
port++;

let apiUrl : string = document.location.protocol + '//' + document.location.hostname + ':' + port;
console.log(apiUrl);

async function fetchContent() {
  const resp : Response = await fetch(document.baseURI + "/api/get-images", {method: "GET"});
  console.log(resp);

  try {
    const json = await resp.json();
    console.log(json);
  } catch (error) {
    console.log(error)   
  }
}

// fetchContent()

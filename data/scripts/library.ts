const API_URL: string = document.location.protocol + "//" + document.location.hostname + ":" + (Number(document.location.port) + 1);

interface FoldersConfigObject {
  uri: string;
}

interface FoldersConfigJSON {
  version: string;
  folders: FoldersConfigObject[];
}

interface FolderDataObject {
  uri: string;
}

interface FolderDataJSON {
  version: string;
  images: FolderDataObject[];
}

async function fetchFoldersHTML(): Promise<string[]> {
  let output: string[] = [];
  let foldersConfigJSON: FoldersConfigJSON = await (
    await fetch(API_URL + "/api/folders", { method: "GET" })
  ).json();

  for (let i = 0; i < foldersConfigJSON.folders.length; i++) {
    let folderDataJSON: FolderDataJSON = await (
      await fetch(API_URL.toString() + foldersConfigJSON.folders[i].uri, {
        method: "GET",
      })
    ).json();

    if (folderDataJSON.images.length <= 0) {
      continue;
    }

    let uriComponents: string[] = foldersConfigJSON.folders[i].uri.split("/");
    let dateString: string = uriComponents[uriComponents.length - 1].substring(0, 10);

    let folderViewImagesHTML : string = "";

    for (let i = 0; i < folderDataJSON.images.length; i++) {
      const image = folderDataJSON.images[i];

      const folderViewImageItemHTML = `
        <li><a href="${API_URL + image.uri}"><img src="${API_URL + image.uri}" alt="${image.uri}" loading="lazy"></a></li>
      `

      folderViewImagesHTML = folderViewImagesHTML.concat(folderViewImageItemHTML.trim());
    }

    const folderTileHTML = `
      <li class="folder-list-item" id="${dateString.trim()}">
        <h2>${dateString.trim()}</h2>
        <img src="${API_URL + folderDataJSON.images[0].uri}" alt="${(API_URL + folderDataJSON.images[0].uri).trim()}">
        <ul id="${dateString.trim()}-view" style="display: none;">
          ${folderViewImagesHTML.trim()}
        </ul>
        <div>
          <button><h3>View</h3></button>
          <button><h3>Download</h3></button>
        </div>
      </li>
    `;

    output.push(folderTileHTML.trim());
  }

  return output;
}

function downloadClick(date : string): void {
  const dateTile : HTMLElement | null = document.getElementById(date);
}

function viewClick(date: string): void {
  const dateTile : HTMLElement | null = document.getElementById(date);

  const imageElements = dateTile?.getElementsByTagName("img");
  const unorderedListElements = dateTile?.getElementsByTagName("ul");
  
  if (!imageElements || !unorderedListElements) {
    return
  }

  if (imageElements.length <= 0 || unorderedListElements.length <= 0) {
    return
  }

  const imageElement = imageElements[0];
  const unorderedListElement = unorderedListElements[0];

  if (imageElement.style.getPropertyValue("display") == "none") {
    imageElement.style.setProperty("display", "initial");
    unorderedListElement.style.setProperty("display", "none");
  }
  else {
    imageElement.style.setProperty("display", "none");
    unorderedListElement.style.setProperty("display", "flex");
  }
}

function folderTileClick(event: Event) {
  const target: HTMLElement = event.target as HTMLElement;
  const dateString: string | undefined = target.closest("li")?.id;

  if (target.closest("button")?.textContent?.includes("View")) {
    if (dateString) {
      viewClick(dateString);
    }
  } else if (target.closest("button")?.textContent?.includes("Download")) {
    if (dateString) {
      downloadClick(dateString);
    }
  }
}

fetchFoldersHTML().then((foldersHTML: string[]) => {
  const folderList: HTMLElement | null = document.getElementById("main-folder-list");

  for (const snippet of foldersHTML) {
    folderList?.insertAdjacentHTML("beforeend", snippet);
  }

  folderList?.addEventListener("click", folderTileClick);
});

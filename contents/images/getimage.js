

for (let i = 90; i < 130; i++)
{
	let filepath = `https://images-na.ssl-images-amazon.com/images/G/01/error/${i}._TTD_.jpg`
	fileDownloadFromUrl(filepath);

}


async function fileDownloadFromUrl(fileName, fileUrl) {
	const response = await fetch(fileUrl);
	const blob = await response.blob();
	const newBlob = new Blob([blob]);
	const objUrl = window.URL.createObjectURL(newBlob);
	const link = document.createElement("a");
	link.href = objUrl;
	link.download = fileName;
	link.click();
	// For Firefox it is necessary to delay revoking the ObjectURL.
	setTimeout(() => {
	  window.URL.revokeObjectURL(objUrl);
	}, 250);
  }
  for(let i = 90; i < 120; i++)
  {
	let filepath = `https://images-na.ssl-images-amazon.com/images/G/01/error/${i}._TTD_.jpg`
	downloadImage(filepath);
  }

  function downloadImage(url){
	const matches = url.match(/[A-Za-z0-9\-_]+\.\w+$/);
   
	fetch(url, {
	  method: 'GET',
	  headers: {},
	})
	  .then((response) => {
		response.arrayBuffer().then((buffer) => {
		  const url = window.URL.createObjectURL(new Blob([buffer]));
		  const link = document.createElement('a');
		  link.href = url;
		  link.setAttribute('download', matches[0]);
		  document.body.appendChild(link);
		  link.click();
		});
	  })
	  .catch((err) => {
		console.log(err);
	  });
  }
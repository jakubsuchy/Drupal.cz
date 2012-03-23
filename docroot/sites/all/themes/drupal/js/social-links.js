function showSocials(i) {
	var links = document.getElementById("social-links-" + i);
	links.style.display = 'block';
}
function hideSocials(i) {
	hideSocialsReal(i);
}
function hideSocialsReal(i) {
	var links = document.getElementById("social-links-" + i);
	links.style.display = 'none';

}

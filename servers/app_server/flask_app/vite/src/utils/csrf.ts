export function getCsrfToken() {
  const cookieValue = document.cookie
    .split("; ")
    .find((row) => row.startsWith("csrf_token="))
    ?.split("=")[1];
  return cookieValue;
}

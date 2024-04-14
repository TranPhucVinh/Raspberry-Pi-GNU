from http.server import HTTPServer, BaseHTTPRequestHandler

return_http_server_message = {
    "msg": "success"
}
class Server(BaseHTTPRequestHandler):
	def do_GET(self): #do_GET is a built-in method and can't be renamed
		if self.path == "/":
			self.send_response(200)
			self.end_headers()   
            #A bytes-like object is required, not 'str'
			self.wfile.write(bytes("Hello World", "utf-8"))

	def do_POST(self):
		if self.path == "/api/activity":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()
			self.wfile.write(bytes(str(return_http_server_message), "utf-8"))

			#Print out received data from HTTP Client
			#Gets the size of data
			content_length = int(self.headers['Content-Length'])
			received_data = self.rfile.read(content_length)
			print("Data: ", received_data.decode('utf-8'))

http = HTTPServer(("localhost", 8080), Server)
http.serve_forever()
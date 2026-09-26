#!/usr/bin/env python3
"""Cross-origin isolated HTTP server for Godot Web export builds."""
import os
import sys
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer

class Handler(SimpleHTTPRequestHandler):
    extensions_map = {
        **SimpleHTTPRequestHandler.extensions_map,
        ".js": "text/javascript",
        ".mjs": "text/javascript",
        ".wasm": "application/wasm",
        ".pck": "application/octet-stream",
        ".bin": "application/octet-stream",
        ".html": "text/html",
        ".json": "application/json",
        ".svg": "image/svg+xml",
        ".png": "image/png",
    }

    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cross-Origin-Resource-Policy", "same-origin")
        self.send_header("Cache-Control", "no-cache, no-store, must-revalidate")
        super().end_headers()

    def log_message(self, fmt, *args):
        sys.stderr.write("  " + (fmt % args) + "\n")

def main():
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8080
    build_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "builds", "web")
    if not os.path.isdir(build_dir):
        print(f"Directory not found: {build_dir}")
        sys.exit(1)
    httpd = ThreadingHTTPServer(("127.0.0.1", port), partial(Handler, directory=build_dir))
    print(f"Serving {build_dir} on http://localhost:{port}/index.html")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nStopped.")

if __name__ == "__main__":
    main()

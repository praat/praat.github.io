/**
 * Simple local HTTP server for browser integration testing.
 * Serves the project root with correct MIME types for WASM and ES modules.
 *
 * Usage: node docs/serve-test.mjs
 * Then open: http://localhost:8000/docs/wasm-test.html
 */

import { createServer } from 'node:http'
import { readFile, stat } from 'node:fs/promises'
import { join, extname } from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = fileURLToPath(new URL('../..', import.meta.url))
const PORT = 8000

const MIME_TYPES = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'application/javascript; charset=utf-8',
  '.mjs': 'application/javascript; charset=utf-8',
  '.wasm': 'application/wasm',
  '.css': 'text/css; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.png': 'image/png',
  '.wav': 'audio/wav',
  '.svg': 'image/svg+xml'
}

const server = createServer(async (req, res) => {
  const url = new URL(req.url, `http://localhost:${PORT}`)
  let filePath = join(__dirname, decodeURIComponent(url.pathname))

  /* Prevent directory traversal */
  if (!filePath.startsWith(__dirname)) {
    res.writeHead(403)
    res.end('Forbidden')
    return
  }

  try {
    const s = await stat(filePath)
    if (s.isDirectory()) filePath = join(filePath, 'index.html')

    const ext = extname(filePath).toLowerCase()
    const contentType = MIME_TYPES[ext] || 'application/octet-stream'
    const data = await readFile(filePath)

    res.writeHead(200, {
      'Content-Type': contentType,
      'Cross-Origin-Opener-Policy': 'same-origin',
      'Cross-Origin-Embedder-Policy': 'require-corp',
      'Cache-Control': 'no-cache'
    })
    res.end(data)
  } catch (e) {
    if (e.code === 'ENOENT') {
      res.writeHead(404)
      res.end('Not Found: ' + url.pathname)
    } else {
      res.writeHead(500)
      res.end('Server Error: ' + e.message)
    }
  }
})

server.listen(PORT, () => {
  console.log(`Serving from: ${__dirname}`)
  console.log(`Open: http://localhost:${PORT}/docs/wasm-test/`)
})

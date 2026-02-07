#!/usr/bin/env python3
"""
PyAE REPL Client
================
After Effects内で動作しているPyAE REPLサーバーに接続するクライアント

使用方法:
    python repl_client.py [port] [token]

例:
    python repl_client.py 9999 0a3dd0f5a0ae97c905016a3aadc04c97
"""

import socket
import sys
import threading
import time
from typing import Optional, List


def connect_repl(host: str = "127.0.0.1", port: int = 9999, token: str = "") -> Optional[socket.socket]:
    """REPLサーバーに接続"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        print(f"Connected to PyAE REPL at {host}:{port}")

        # 認証
        if token:
            auth_msg = f"AUTH {token}\n"
            sock.sendall(auth_msg.encode('utf-8'))
            response = sock.recv(1024).decode('utf-8')
            if "OK" in response:
                print("Authentication successful")
            else:
                print(f"Authentication failed: {response}")
                sock.close()
                return None

        return sock
    except ConnectionRefusedError:
        print(f"Connection refused. Is After Effects running with PyAE loaded?")
        return None
    except Exception as e:
        print(f"Connection error: {e}")
        return None


def receive_thread(sock: socket.socket, running: List[bool]) -> None:
    """受信スレッド"""
    sock.settimeout(0.5)
    while running[0]:
        try:
            data = sock.recv(4096)
            if data:
                print(data.decode('utf-8'), end='', flush=True)
            else:
                break
        except socket.timeout:
            continue
        except Exception as e:
            if running[0]:
                print(f"\nReceive error: {e}")
            break


def repl_session(sock: socket.socket) -> None:
    """対話的REPLセッション"""
    running = [True]

    # 受信スレッドを開始
    recv_thread = threading.Thread(target=receive_thread, args=(sock, running))
    recv_thread.daemon = True
    recv_thread.start()

    print("\n" + "=" * 50)
    print("PyAE REPL Session")
    print("=" * 50)
    print("Type Python code to execute in After Effects")
    print("Special commands:")
    print("  exit    - Close connection")
    print("  quit    - Close connection")
    print("  help    - Show help")
    print("=" * 50 + "\n")

    try:
        while True:
            try:
                code = input(">>> ")
            except EOFError:
                break

            if code.lower() in ('exit', 'quit'):
                break

            if code.lower() == 'help':
                print("""
PyAE REPL Help
==============
Available modules:
  import ae  - Access After Effects API

Examples:
  # Get active project
  proj = ae.get_project()
  print(proj.name)

  # Get active composition
  comp = ae.get_active_comp()
  if comp:
      print(f"Comp: {comp.name}, {comp.width}x{comp.height}")

  # List layers
  for i in range(comp.num_layers):
      layer = comp.layer(i + 1)
      print(f"Layer {i+1}: {layer.name}")

  # Create solid
  solid = comp.add_solid("My Solid", 100, 100, (1, 0, 0), 5.0)

  # Modify layer
  layer = comp.layer(1)
  layer.position = (960, 540, 0)
  layer.opacity = 50.0
""")
                continue

            if code.strip():
                # コードを送信
                sock.sendall((code + "\n").encode('utf-8'))
                time.sleep(0.1)  # 応答を待つ

    except KeyboardInterrupt:
        print("\nInterrupted")
    finally:
        running[0] = False
        sock.close()
        print("Connection closed")


def main() -> None:
    port = 9999
    token = ""

    if len(sys.argv) >= 2:
        port = int(sys.argv[1])
    if len(sys.argv) >= 3:
        token = sys.argv[2]

    # トークンが指定されていない場合は環境変数を確認
    if not token:
        import os
        token = os.environ.get("PYAE_REPL_TOKEN", "")

    print(f"Connecting to PyAE REPL on port {port}...")

    sock = connect_repl(port=port, token=token)
    if sock:
        repl_session(sock)


if __name__ == '__main__':
    main()

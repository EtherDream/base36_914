emcc demo.c \
  -O3 \
  -g2 \
  -o demo.wasm \
  -s TOTAL_MEMORY=24MB \
  --no-entry
import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

export default defineConfig({
    plugins: [react()],
    base: './',
    build: {
        outDir: 'dist',
        assetsInlineLimit: 100000,
        cssCodeSplit: false,
        rollupOptions: {
            output: {
                manualChunks: undefined,
                inlineDynamicImports: true,
            }
        }
    },
    server: {
        port: 5173,
        strictPort: true,
    }
});

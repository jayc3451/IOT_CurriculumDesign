import { createApp } from 'vue'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import App from './App.vue'
import router from './router/router'
// import './assets/main.css'

const app = createApp(App)
app.use(ElementPlus)        // 全局注册使用
app.use(router)             // 全局注册使用

app.mount('#app')
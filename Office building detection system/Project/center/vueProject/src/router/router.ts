// /src/router/router.ts
// 引入 vue-router
import { createRouter, createWebHistory } from 'vue-router'
import AppLayout from '@/layout/index.vue'

const routes = [
  {
    path: '/',
    // name: '/',
    redirect: '/home',
    component: AppLayout,
    children: [
      {
        path: '/home',
        name: 'Home',
        component: () => import('@/views/Home.vue')
      },
    ]
  },
  // {
  //   path: '/home',
  //   component: AppLayout,
  //   redirect: '/home',
  //   children: [
  //     {
  //       path: '/home',
  //       component: () => import('@/views/Home.vue')
  //     }
  //   ]
  // }
]
// createRouter创建路由
const router = createRouter({
  history: createWebHistory(),
  routes,
})

// // 使用路由导航守卫来动态更改标题
// router.afterEach((to, from) => {
//   document.title = (to.meta && to.meta.title) ? to.meta.title as string : '首页';
// });
export default router

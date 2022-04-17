from django.urls import path
from django.contrib.auth import views as login
from . import views

app_name = 'user'
urlpatterns = [
    path('', views.welcome, name='welcome'),
    path('login/', login.LoginView.as_view(template_name='user/login.html'), name='login'),
    path('logout/', login.LogoutView.as_view(template_name='user/logout.html'), name='logout'),
    path('user_info/change/', views.change_info, name='change_info'), 
    path('user_info/', views.show_info, name='show_info'), 
    path('create_account/', views.create_account, name='create_account'), 
]

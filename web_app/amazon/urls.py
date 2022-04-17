from django.urls import path
from . import views

app_name = 'amazon'
urlpatterns = [
    path('home/', views.home, name='home'),
    path('category/', views.search_by_cat, name='search_by_cat'),
    path('cart/', views.goto_cart, name='goto_cart'),
    path('order/', views.view_order, name='view_order'),
    path('product/<int:p_id>/', views.view_product, name='view_product'),
]

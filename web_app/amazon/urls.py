from django.urls import path
from . import views

app_name = 'amazon'
urlpatterns = [
    path('home/', views.home, name='home'),
    path('category/', views.search_by_cat, name='search_by_cat'),
    path('cart/', views.goto_cart, name='goto_cart'),
    path('order/', views.view_order, name='view_order'),
    path('product/<int:p_id>/', views.view_product, name='view_product'),
    path('checkout/<int:o_id>/', views.checkout, name='checkout'),
    path('order_detail/<int:o_id>/', views.view_order_detail, name='view_order_detail'),
    path('change_item/<int:i_id>/', views.change_item, name='change_item'),
    path('category/<int:c_id>/', views.view_cat, name='view_cat'),
]
